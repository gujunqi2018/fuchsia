// Copyright 2020 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

use {
    crate::{
        builtin::capability::{BuiltinCapability, BuiltinCapabilityProvider},
        capability::*,
        model::{
            error::ModelError,
            hooks::{Event, EventPayload, Hook},
        },
    },
    anyhow::Error,
    async_trait::async_trait,
    cm_rust::CapabilityPath,
    fidl_fuchsia_boot as fboot,
    fuchsia_zircon::{self as zx, DebugLog, DebugLogOpts, HandleBased, Resource},
    futures::prelude::*,
    lazy_static::lazy_static,
    std::{convert::TryInto, sync::Arc},
};

lazy_static! {
    static ref READ_ONLY_LOG_CAPABILITY_PATH: CapabilityPath =
        "/svc/fuchsia.boot.ReadOnlyLog".try_into().unwrap();
    static ref WRITE_ONLY_LOG_CAPABILITY_PATH: CapabilityPath =
        "/svc/fuchsia.boot.WriteOnlyLog".try_into().unwrap();
}

/// An implementation of the `fuchsia.boot.ReadOnlyLog` protocol.
pub struct ReadOnlyLog {
    resource: Resource,
}

impl ReadOnlyLog {
    /// Create a service to provide a read-only version of the kernel log.
    /// Note that the root resource is passed in here, rather than a read-only log handle to be
    /// duplicated, because a fresh debuglog (LogDispatcher) object needs to be returned for each call.
    /// This is because LogDispatcher holds the implicit read location for reading from the log, so if a
    /// handle to the same object was duplicated, this would mistakenly share read location amongst all
    /// retrievals.
    pub fn new(resource: Resource) -> Arc<Self> {
        Arc::new(Self { resource })
    }
}

#[async_trait]
impl BuiltinCapability for ReadOnlyLog {
    const NAME: &'static str = "ReadOnlyLog";
    type Marker = fboot::ReadOnlyLogMarker;

    async fn serve(
        self: Arc<Self>,
        mut stream: fboot::ReadOnlyLogRequestStream,
    ) -> Result<(), Error> {
        while let Some(fboot::ReadOnlyLogRequest::Get { responder }) = stream.try_next().await? {
            let debuglog = DebugLog::create(&self.resource, DebugLogOpts::READABLE)?;
            // If `READABLE` is set, then the rights for the debuglog include `READ`. However, we
            // must remove the `WRITE` right, as we want to provide a read-only debuglog.
            let readonly = debuglog
                .replace_handle(zx::Rights::BASIC | zx::Rights::READ | zx::Rights::SIGNAL)?;
            responder.send(readonly)?;
        }
        Ok(())
    }

    async fn on_framework_capability_routed<'a>(
        self: &'a Arc<Self>,
        capability: &'a FrameworkCapability,
        capability_provider: Option<Box<dyn CapabilityProvider>>,
    ) -> Result<Option<Box<dyn CapabilityProvider>>, ModelError> {
        match capability {
            FrameworkCapability::Protocol(capability_path)
                if *capability_path == *READ_ONLY_LOG_CAPABILITY_PATH =>
            {
                Ok(Some(Box::new(BuiltinCapabilityProvider::<ReadOnlyLog>::new(Arc::downgrade(
                    &self,
                )))))
            }
            _ => Ok(capability_provider),
        }
    }
}

#[async_trait]
impl Hook for ReadOnlyLog {
    async fn on(self: Arc<Self>, event: &Event) -> Result<(), ModelError> {
        if let EventPayload::CapabilityRouted {
            source: CapabilitySource::Framework { capability, scope_moniker: None },
            capability_provider,
        } = &event.payload
        {
            let mut provider = capability_provider.lock().await;
            *provider = self.on_framework_capability_routed(&capability, provider.take()).await?;
        };
        Ok(())
    }
}

/// An implementation of the `fuchsia.boot.WriteOnlyLog` protocol.
pub struct WriteOnlyLog {
    debuglog: DebugLog,
}

impl WriteOnlyLog {
    pub fn new(debuglog: DebugLog) -> Arc<Self> {
        Arc::new(Self { debuglog })
    }
}

#[async_trait]
impl BuiltinCapability for WriteOnlyLog {
    const NAME: &'static str = "WriteOnlyLog";
    type Marker = fboot::WriteOnlyLogMarker;

    async fn serve(
        self: Arc<Self>,
        mut stream: fboot::WriteOnlyLogRequestStream,
    ) -> Result<(), Error> {
        while let Some(fboot::WriteOnlyLogRequest::Get { responder }) = stream.try_next().await? {
            responder.send(self.debuglog.duplicate_handle(zx::Rights::SAME_RIGHTS)?)?;
        }
        Ok(())
    }

    async fn on_framework_capability_routed<'a>(
        self: &'a Arc<Self>,
        capability: &'a FrameworkCapability,
        capability_provider: Option<Box<dyn CapabilityProvider>>,
    ) -> Result<Option<Box<dyn CapabilityProvider>>, ModelError> {
        match capability {
            FrameworkCapability::Protocol(capability_path)
                if *capability_path == *WRITE_ONLY_LOG_CAPABILITY_PATH =>
            {
                Ok(Some(Box::new(BuiltinCapabilityProvider::<WriteOnlyLog>::new(Arc::downgrade(
                    &self,
                )))))
            }
            _ => Ok(capability_provider),
        }
    }
}

#[async_trait]
impl Hook for WriteOnlyLog {
    async fn on(self: Arc<Self>, event: &Event) -> Result<(), ModelError> {
        if let EventPayload::CapabilityRouted {
            source: CapabilitySource::Framework { capability, scope_moniker: None },
            capability_provider,
        } = &event.payload
        {
            let mut provider = capability_provider.lock().await;
            *provider = self.on_framework_capability_routed(&capability, provider.take()).await?;
        };
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use {
        super::*,
        crate::model::{hooks::Hooks, moniker::AbsoluteMoniker},
        fidl::endpoints::ClientEnd,
        fuchsia_async as fasync,
        fuchsia_zircon::AsHandleRef,
        fuchsia_zircon_sys as sys,
        futures::lock::Mutex,
        std::path::PathBuf,
    };

    #[fasync::run_singlethreaded(test)]
    async fn has_correct_rights_for_read_only() -> Result<(), Error> {
        // The kernel does not currently require a valid `Resource` to be
        // provided when creating a `Debuglog`. This will change in the future.
        let read_only_log = ReadOnlyLog::new(Resource::from(zx::Handle::invalid()));
        let (proxy, stream) =
            fidl::endpoints::create_proxy_and_stream::<fboot::ReadOnlyLogMarker>()?;
        fasync::spawn_local(
            read_only_log
                .serve(stream)
                .unwrap_or_else(|err| panic!("Error serving read-only log: {}", err)),
        );

        let read_only_log = proxy.get().await?;
        let info = zx::Handle::from(read_only_log).basic_info()?;
        assert_eq!(info.rights, zx::Rights::BASIC | zx::Rights::READ | zx::Rights::SIGNAL);
        Ok(())
    }

    #[fasync::run_singlethreaded(test)]
    async fn can_connect_to_read_only() -> Result<(), Error> {
        // The kernel does not currently require a valid `Resource` to be
        // provided when creating a `Debuglog`. This will change in the future.
        let read_only_log = ReadOnlyLog::new(Resource::from(zx::Handle::invalid()));
        let hooks = Hooks::new(None);
        hooks.install(read_only_log.hooks()).await;

        let provider = Arc::new(Mutex::new(None));
        let source = CapabilitySource::Framework {
            capability: FrameworkCapability::Protocol(READ_ONLY_LOG_CAPABILITY_PATH.clone()),
            scope_moniker: None,
        };

        let event = Event::new(
            AbsoluteMoniker::root(),
            EventPayload::CapabilityRouted { source, capability_provider: provider.clone() },
        );
        hooks.dispatch(&event).await?;

        let (client, server) = zx::Channel::create()?;
        if let Some(provider) = provider.lock().await.take() {
            provider.open(0, 0, PathBuf::new(), server).await?;
        }

        let client = ClientEnd::<fboot::ReadOnlyLogMarker>::new(client)
            .into_proxy()
            .expect("Failed to create proxy");
        let handle = client.get().await?;
        assert_ne!(handle.raw_handle(), sys::ZX_HANDLE_INVALID);

        let mut record = Vec::new();
        handle.read(&mut record)?;
        let message: [u8; 0] = [];
        assert!(handle.write(&message).is_err());

        Ok(())
    }

    #[fasync::run_singlethreaded(test)]
    async fn has_correct_rights_for_write_only() -> Result<(), Error> {
        // The kernel does not currently require a valid `Resource` to be
        // provided when creating a `Debuglog`. This will change in the future.
        let resource = Resource::from(zx::Handle::invalid());
        let write_only_log =
            WriteOnlyLog::new(zx::DebugLog::create(&resource, zx::DebugLogOpts::empty()).unwrap());
        let (proxy, stream) =
            fidl::endpoints::create_proxy_and_stream::<fboot::WriteOnlyLogMarker>()?;
        fasync::spawn_local(
            write_only_log
                .serve(stream)
                .unwrap_or_else(|err| panic!("Error serving write-only log: {}", err)),
        );

        let write_only_log = proxy.get().await?;
        let info = zx::Handle::from(write_only_log).basic_info()?;
        assert_eq!(info.rights, zx::Rights::BASIC | zx::Rights::WRITE | zx::Rights::SIGNAL);

        Ok(())
    }

    #[fasync::run_singlethreaded(test)]
    async fn can_connect_to_write_only() -> Result<(), Error> {
        // The kernel does not currently require a valid `Resource` to be
        // provided when creating a `Debuglog`. This will change in the future.
        let resource = Resource::from(zx::Handle::invalid());
        let write_only_log =
            WriteOnlyLog::new(zx::DebugLog::create(&resource, zx::DebugLogOpts::empty()).unwrap());
        let hooks = Hooks::new(None);
        hooks.install(write_only_log.hooks()).await;

        let provider = Arc::new(Mutex::new(None));
        let source = CapabilitySource::Framework {
            capability: FrameworkCapability::Protocol(WRITE_ONLY_LOG_CAPABILITY_PATH.clone()),
            scope_moniker: None,
        };

        let event = Event::new(
            AbsoluteMoniker::root(),
            EventPayload::CapabilityRouted { source, capability_provider: provider.clone() },
        );
        hooks.dispatch(&event).await?;

        let (client, server) = zx::Channel::create()?;
        if let Some(provider) = provider.lock().await.take() {
            provider.open(0, 0, PathBuf::new(), server).await?;
        }

        let client = ClientEnd::<fboot::WriteOnlyLogMarker>::new(client)
            .into_proxy()
            .expect("Failed to create proxy");
        let handle = client.get().await?;
        assert_ne!(handle.raw_handle(), sys::ZX_HANDLE_INVALID);

        let mut record = Vec::new();
        assert!(handle.read(&mut record).is_err());
        let message: [u8; 0] = [];
        handle.write(&message)?;

        Ok(())
    }
}
