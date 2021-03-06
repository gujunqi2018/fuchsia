// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "minfs/minfs-inspector.h"

#include <iostream>

#include <block-client/cpp/fake-device.h>
#include <disk_inspector/inspector_transaction_handler.h>
#include <fs/journal/format.h>
#include <minfs/format.h>
#include <zxtest/zxtest.h>

#include "minfs-private.h"

namespace minfs {
namespace {

using block_client::FakeBlockDevice;

constexpr uint64_t kBlockCount = 1 << 15;
constexpr uint32_t kBlockSize = 512;

// Initialize a MinfsInspector from a created fake block device formatted
// into a fresh minfs partition and journal entries.
void SetupMinfsInspector(std::unique_ptr<MinfsInspector>* inspector) {
  auto temp = std::make_unique<FakeBlockDevice>(kBlockCount, kBlockSize);

  // Format the device.
  std::unique_ptr<Bcache> bcache;
  ASSERT_OK(Bcache::Create(std::move(temp), kBlockCount, &bcache));
  ASSERT_OK(Mkfs(bcache.get()));

  // Write journal info to the device by creating a minfs and waiting for it
  // to finish.
  std::unique_ptr<Minfs> fs;
  MountOptions options = {};
  ASSERT_OK(minfs::Minfs::Create(std::move(bcache), options, &fs));
  sync_completion_t completion;
  fs->Sync([&completion](zx_status_t status) { sync_completion_signal(&completion); });
  ASSERT_OK(sync_completion_wait(&completion, zx::duration::infinite().get()));

  // We only care about the disk format written into the fake block device,
  // so we destroy the minfs/bcache used to format it.
  bcache = Minfs::Destroy(std::move(fs));
  ASSERT_OK(MinfsInspector::Create(Bcache::Destroy(std::move(bcache)), inspector));
}

TEST(MinfsInspector, CreateWithoutError) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);
}

TEST(MinfsInspector, InspectSuperblock) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  Superblock sb = inspector->InspectSuperblock();

  EXPECT_EQ(sb.magic0, kMinfsMagic0);
  EXPECT_EQ(sb.magic1, kMinfsMagic1);
  EXPECT_EQ(sb.version_major, kMinfsMajorVersion);
  EXPECT_EQ(sb.version_minor, kMinfsMinorVersion);
  EXPECT_EQ(sb.flags, kMinfsFlagClean);
  EXPECT_EQ(sb.block_size, kMinfsBlockSize);
  EXPECT_EQ(sb.inode_size, kMinfsInodeSize);
  EXPECT_EQ(sb.alloc_block_count, 2);
  EXPECT_EQ(sb.alloc_block_count, 2);
}

TEST(MinfsInspector, InspectInode) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  Superblock sb = inspector->InspectSuperblock();
  // The fresh minfs device should have 2 allocated inodes, empty inode 0 and
  // allocated inode 1.
  ASSERT_EQ(sb.alloc_inode_count, 2);

  // 0th inode is uninitialized.
  uint32_t i = 0;
  Inode inode = inspector->InspectInode(i);
  EXPECT_EQ(inode.magic, 0);
  EXPECT_EQ(inode.size, 0);
  EXPECT_EQ(inode.block_count, 0);
  EXPECT_EQ(inode.link_count, 0);

  // 1st inode is initialized and is the root directory.
  i = 1;
  inode = inspector->InspectInode(i);
  EXPECT_EQ(inode.magic, kMinfsMagicDir);
  EXPECT_EQ(inode.size, kMinfsBlockSize);
  EXPECT_EQ(inode.block_count, 1);
  EXPECT_EQ(inode.link_count, 2);

  // 2nd inode is uninitialized.
  i = 2;
  inode = inspector->InspectInode(i);
  EXPECT_EQ(inode.magic, 0);
  EXPECT_EQ(inode.size, 0);
  EXPECT_EQ(inode.block_count, 0);
  EXPECT_EQ(inode.link_count, 0);
}

TEST(MinfsInspector, GetInodeCount) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  Superblock sb = inspector->InspectSuperblock();
  EXPECT_EQ(inspector->GetInodeCount(), sb.inode_count);
}

TEST(MinfsInspector, CheckInodeAllocated) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  Superblock sb = inspector->InspectSuperblock();
  ASSERT_TRUE(sb.alloc_inode_count < sb.inode_count);

  uint32_t max_samples = 10;
  uint32_t num_inodes_to_sample = (sb.inode_count < max_samples) ? sb.inode_count : max_samples;

  for (uint32_t i = 0; i < num_inodes_to_sample; ++i) {
    bool is_allocated = inspector->CheckInodeAllocated(i);
    if (i < sb.alloc_inode_count) {
      EXPECT_TRUE(is_allocated);
    } else {
      EXPECT_FALSE(is_allocated);
    }
  }
}

TEST(MinfsInspector, InspectJournalSuperblock) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  fs::JournalInfo journal_info = inspector->InspectJournalSuperblock();

  EXPECT_EQ(journal_info.magic, fs::kJournalMagic);
  EXPECT_EQ(journal_info.start_block, 8);
}

TEST(MinfsInspector, InspectJournalPrefix) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  // First four entry blocks should be header, payload, payload, commit.
  fs::JournalPrefix prefix = inspector->InspectJournalPrefix(0);
  EXPECT_EQ(prefix.magic, fs::kJournalEntryMagic);
  EXPECT_EQ(prefix.sequence_number, 0);
  EXPECT_EQ(prefix.flags, fs::kJournalPrefixFlagHeader);

  prefix = inspector->InspectJournalPrefix(1);
  EXPECT_NE(prefix.magic, fs::kJournalEntryMagic);

  prefix = inspector->InspectJournalPrefix(2);
  EXPECT_NE(prefix.magic, fs::kJournalEntryMagic);

  prefix = inspector->InspectJournalPrefix(3);
  EXPECT_EQ(prefix.magic, fs::kJournalEntryMagic);
  EXPECT_EQ(prefix.sequence_number, 0);
  EXPECT_EQ(prefix.flags, fs::kJournalPrefixFlagCommit);
}

TEST(MinfsInspector, InspectJournalHeader) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  // First four entry blocks should be header, payload, payload, commit.
  fs::JournalHeaderBlock header = inspector->InspectJournalHeader(0);
  EXPECT_EQ(header.prefix.magic, fs::kJournalEntryMagic);
  EXPECT_EQ(header.prefix.sequence_number, 0);
  EXPECT_EQ(header.prefix.flags, fs::kJournalPrefixFlagHeader);
  EXPECT_EQ(header.payload_blocks, 2);
}

TEST(MinfsInspector, InspectJournalCommit) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);

  // First four entry blocks should be header, payload, payload, commit.
  fs::JournalCommitBlock commit = inspector->InspectJournalCommit(3);
  EXPECT_EQ(commit.prefix.magic, fs::kJournalEntryMagic);
  EXPECT_EQ(commit.prefix.sequence_number, 0);
  EXPECT_EQ(commit.prefix.flags, fs::kJournalPrefixFlagCommit);
}

TEST(MinfsInspector, InspectBackupSuperblock) {
  std::unique_ptr<MinfsInspector> inspector;
  SetupMinfsInspector(&inspector);
  Superblock sb;
  ASSERT_OK(inspector->InspectBackupSuperblock(&sb));

  EXPECT_EQ(sb.magic0, kMinfsMagic0);
  EXPECT_EQ(sb.magic1, kMinfsMagic1);
  EXPECT_EQ(sb.version_major, kMinfsMajorVersion);
  EXPECT_EQ(sb.version_minor, kMinfsMinorVersion);
  EXPECT_EQ(sb.flags, kMinfsFlagClean);
  EXPECT_EQ(sb.block_size, kMinfsBlockSize);
  EXPECT_EQ(sb.inode_size, kMinfsInodeSize);
  EXPECT_EQ(sb.alloc_block_count, 2);
  EXPECT_EQ(sb.alloc_block_count, 2);
}

// TODO(fxb/46821): Implement these tests once we have a fake block device
// that can send proper error codes when bad operations are passed in.
// Currently if we send a read beyond device command, the block device
// itself will fail some test checks leading to this case being impossible to
// pass.
TEST(MinfsInspector, GracefulReadBeyondDevice) {}
TEST(MinfsInspector, GracefulReadFvmUnmappedData) {}

}  // namespace
}  // namespace minfs
