// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "inspectable.h"

#include <lib/inspect/testing/cpp/inspect.h>

#include <gmock/gmock.h>

#include "gtest/gtest.h"

namespace bt {

namespace {

using namespace inspect::testing;

template <typename T>
class TestProperty {
 public:
  using ValueCallback = fit::function<void(const T& value)>;
  TestProperty(T value, ValueCallback cb) : value_(value), value_cb_(std::move(cb)) {}

  void Set(const T& value) {
    value_ = value;
    if (value_cb_) {
      value_cb_(value_);
    }
  }

 private:
  T value_;
  fit::function<void(const T& value)> value_cb_;
};

struct TestValue {
  TestValue(int val) : value(val) {}
  void set_value(int val) { value = val; }
  int value;
};

struct StringValue {
  std::string ToString() const { return value; }
  std::string value;
};

}  // namespace

using TestInspectable = Inspectable<int, TestProperty<int>, int>;

TEST(InspectableTest, PropertyValueUpdatedOnConstruction) {
  std::optional<int> prop_value;
  auto prop_cb = [&](auto value) { prop_value = value; };

  TestInspectable inspectable(0, TestProperty<int>(1, std::move(prop_cb)));
  EXPECT_EQ(0, *inspectable);
  ASSERT_TRUE(prop_value.has_value());
  // Property value should not still be 1.
  EXPECT_EQ(0, prop_value.value());
}

TEST(InspectableTest, Set) {
  std::optional<int> prop_value;
  auto prop_cb = [&](auto value) { prop_value = value; };

  TestInspectable inspectable(0, TestProperty<int>(0, std::move(prop_cb)));
  inspectable.Set(1);
  EXPECT_EQ(1, *inspectable);
  ASSERT_TRUE(prop_value.has_value());
  EXPECT_EQ(1, prop_value.value());
}

TEST(InspectableTest, UpdateValueThroughMutable) {
  std::optional<int> prop_value;
  auto prop_cb = [&](auto value) { prop_value = value; };

  Inspectable<TestValue, TestProperty<int>, int> inspectable(
      TestValue(0), TestProperty<int>(0, std::move(prop_cb)),
      [](const TestValue& v) { return v.value; });
  inspectable.Mutable()->set_value(1);
  EXPECT_EQ(1, inspectable->value);
  ASSERT_TRUE(prop_value.has_value());
  EXPECT_EQ(1, prop_value.value());
}

TEST(InspectableTest, ToStringInspectable) {
  std::optional<std::string> prop_value;
  auto prop_cb = [&](auto value) { prop_value = value; };

  ToStringInspectable inspectable(StringValue{""},
                                  TestProperty<std::string>("", std::move(prop_cb)));

  const auto kExpectedValue = "fuchsia";
  inspectable.Mutable()->value = kExpectedValue;
  EXPECT_EQ(kExpectedValue, inspectable->value);
  ASSERT_TRUE(prop_value.has_value());
  EXPECT_EQ(kExpectedValue, prop_value.value());
}

TEST(InspectableTest, InspectRealStringProperty) {
  const auto kPropertyName = "test_property";

  inspect::Inspector inspector;
  auto& root = inspector.GetRoot();
  Inspectable inspectable(std::string("A"), root.CreateString(kPropertyName, ""));

  auto hierarchy = inspect::ReadFromVmo(inspector.DuplicateVmo());
  ASSERT_TRUE(hierarchy.is_ok());
  EXPECT_THAT(hierarchy.take_value(),
              AllOf(NodeMatches(PropertyList(ElementsAre(StringIs(kPropertyName, "A"))))));

  inspectable.Set("B");

  hierarchy = inspect::ReadFromVmo(inspector.DuplicateVmo());
  ASSERT_TRUE(hierarchy.is_ok());
  EXPECT_THAT(hierarchy.take_value(),
              AllOf(NodeMatches(PropertyList(ElementsAre(StringIs(kPropertyName, "B"))))));
}

}  // namespace bt
