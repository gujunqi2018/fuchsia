// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

use {
    crate::input_device::{self, InputDeviceBinding},
    crate::utils::Position,
    anyhow::{format_err, Error},
    async_trait::async_trait,
    fidl_fuchsia_input_report as fidl_input_report,
    fidl_fuchsia_input_report::{InputDeviceProxy, InputReport},
    fuchsia_syslog::fx_log_err,
    futures::channel::mpsc::Sender,
    std::collections::HashSet,
    std::iter::FromIterator,
};

pub type MouseButton = u8;

/// A [`MouseEvent`] represents a pointer event with a specified phase, and the buttons
/// involved in said phase. The supported phases for mice include Up, Down, and Move.
///
/// # Example
/// The following MouseEvent represents a movement of 40 units in the x axis and 20 units
/// in the y axis while hold the primary button down.
///
/// ```
/// let mouse_device_event = input_device::InputDeviceEvent::Mouse({
///     MouseEvent::new (
///         movement: Position { x: 40.0, y: 20.0 },
///         phase: fidl_fuchsia_ui_input::PointerEventPhase::Move,
///         buttons: 1,
///     )
/// });
/// ```
#[derive(Clone, Debug, PartialEq)]
pub struct MouseEvent {
    /// The movement in the x axis.
    pub movement_x: i64,

    /// The movement in the y axis.
    pub movement_y: i64,

    /// The phase of the [`buttons`] associated with this input event.
    pub phase: fidl_fuchsia_ui_input::PointerEventPhase,

    /// The buttons relevant to this event.
    pub buttons: HashSet<MouseButton>,
}

impl MouseEvent {
    pub fn new(
        movement: Position,
        phase: fidl_fuchsia_ui_input::PointerEventPhase,
        buttons: HashSet<MouseButton>,
    ) -> Self {
        Self { movement_x: movement.x as i64, movement_y: movement.y as i64, phase, buttons }
    }

    pub fn movement(&self) -> Position {
        Position { x: self.movement_x as f32, y: self.movement_y as f32 }
    }
}

/// A [`MouseBinding`] represents a connection to a mouse input device.
///
/// The [`MouseBinding`] parses and exposes mouse descriptor properties (e.g., the range of
/// possible x values) for the device it is associated with. It also parses [`InputReport`]s
/// from the device, and sends them to clients via [`MouseBinding::input_event_stream()`].
///
/// # Example
/// ```
/// let mut mouse_device: MouseBinding = input_device::InputDeviceBinding::new().await?;
///
/// while let Some(report) = mouse_device.input_event_stream().next().await {}
/// ```
pub struct MouseBinding {
    /// The channel to stream InputEvents to.
    event_sender: Sender<input_device::InputEvent>,

    /// Holds information about this device. Currently empty because no information is needed for
    /// the supported use cases.
    device_descriptor: MouseDeviceDescriptor,
}

#[derive(Copy, Clone, Debug, PartialEq)]
pub struct MouseDeviceDescriptor {
    /// The id of the connected mouse input device.
    pub device_id: u32,
}

#[async_trait]
impl input_device::InputDeviceBinding for MouseBinding {
    fn input_event_sender(&self) -> Sender<input_device::InputEvent> {
        self.event_sender.clone()
    }

    fn get_device_descriptor(&self) -> input_device::InputDeviceDescriptor {
        input_device::InputDeviceDescriptor::Mouse(self.device_descriptor.clone())
    }
}

impl MouseBinding {
    /// Creates a new [`InputDeviceBinding`] from the `device_proxy`.
    ///
    /// The binding will start listening for input reports immediately and send new InputEvents
    /// to the InputPipeline over `input_event_sender`.
    ///
    /// # Parameters
    /// - `device_proxy`: The proxy to bind the new [`InputDeviceBinding`] to.
    /// - `input_event_sender`: The channel to send new InputEvents to.
    ///
    /// # Errors
    /// If there was an error binding to the proxy.
    pub async fn new(
        device_proxy: InputDeviceProxy,
        input_event_sender: Sender<input_device::InputEvent>,
    ) -> Result<Self, Error> {
        let device_binding = Self::bind_device(&device_proxy, input_event_sender).await?;
        input_device::initialize_report_stream(
            device_proxy,
            device_binding.get_device_descriptor(),
            device_binding.input_event_sender(),
            Self::process_reports,
        );

        Ok(device_binding)
    }

    /// Binds the provided input device to a new instance of `Self`.
    ///
    /// # Parameters
    /// - `device`: The device to use to initalize the binding.
    /// - `input_event_sender`: The channel to send new InputEvents to.
    ///
    /// # Errors
    /// If the device descriptor could not be retrieved, or the descriptor could
    /// not be parsed correctly.
    async fn bind_device(
        device: &InputDeviceProxy,
        input_event_sender: Sender<input_device::InputEvent>,
    ) -> Result<Self, Error> {
        let device_descriptor: fidl_input_report::DeviceDescriptor =
            device.get_descriptor().await?;
        match device_descriptor.mouse {
            Some(_) => {
                let device_id = 0;
                let device_descriptor: MouseDeviceDescriptor = MouseDeviceDescriptor { device_id };

                Ok(MouseBinding { event_sender: input_event_sender, device_descriptor })
            }
            descriptor => Err(format_err!("Mouse Descriptor failed to parse: \n {:?}", descriptor)),
        }
    }

    /// Parses an [`InputReport`] into one or more [`InputEvent`]s.
    ///
    /// The [`InputEvent`]s are sent to the device binding owner via [`sender`].
    ///
    /// # Parameters
    /// `report`: The incoming [`InputReport`].
    /// `previous_report`: The previous [`InputReport`] seen for the same device. This can be
    ///                    used to determine, for example, which keys are no longer present in
    ///                    a keyboard report to generate key released events. If `None`, no
    ///                    previous report was found.
    /// `device_descriptor`: The descriptor for the input device generating the input reports.
    /// `input_event_sender`: The sender for the device binding's input event stream.
    ///
    /// # Returns
    /// An [`InputReport`] which will be passed to the next call to [`process_reports`], as
    /// [`previous_report`]. If `None`, the next call's [`previous_report`] will be `None`.
    fn process_reports(
        report: InputReport,
        previous_report: Option<InputReport>,
        device_descriptor: &input_device::InputDeviceDescriptor,
        input_event_sender: &mut Sender<input_device::InputEvent>,
    ) -> Option<InputReport> {
        // Input devices can have multiple types so ensure `report` is a MouseInputReport.
        let mouse_report: &fidl_input_report::MouseInputReport = match &report.mouse {
            Some(mouse) => mouse,
            None => {
                return previous_report;
            }
        };

        let previous_buttons: HashSet<MouseButton> =
            buttons_from_optional_report(&previous_report.as_ref());
        let current_buttons: HashSet<MouseButton> = buttons_from_report(&report);

        let event_time: input_device::EventTime =
            input_device::event_time_or_now(report.event_time);

        // Down events are sent for the buttons which did not exist in the previous report.
        send_mouse_event(
            Position { x: 0.0, y: 0.0 },
            fidl_fuchsia_ui_input::PointerEventPhase::Down,
            current_buttons.difference(&previous_buttons).cloned().collect(),
            device_descriptor,
            event_time,
            input_event_sender,
        );

        // Move events are sent for both previous and current buttons.
        send_mouse_event(
            Position {
                x: mouse_report.movement_x.unwrap_or_default() as f32,
                y: mouse_report.movement_y.unwrap_or_default() as f32,
            },
            fidl_fuchsia_ui_input::PointerEventPhase::Move,
            current_buttons.union(&previous_buttons).cloned().collect(),
            device_descriptor,
            event_time,
            input_event_sender,
        );

        // Up events are sent for previous buttons that are no longer present in the current buttons.
        send_mouse_event(
            Position { x: 0.0, y: 0.0 },
            fidl_fuchsia_ui_input::PointerEventPhase::Up,
            previous_buttons.difference(&current_buttons).cloned().collect(),
            device_descriptor,
            event_time,
            input_event_sender,
        );

        Some(report)
    }
}

/// Sends an InputEvent over `sender`.
///
/// When no buttons are present, only [`fidl_fuchsia_ui_input::PointerEventPhase::Move`] events will
/// be sent.
///
/// # Parameters
/// - `movement`: The movement.
/// - `phase`: The phase of the [`buttons`] associated with the input event.
/// - `buttons`: The buttons relevant to the event.
/// - `device_descriptor`: The descriptor for the input device generating the input reports.
/// - `event_time`: The time in nanoseconds when the event was first recorded.
/// - `sender`: The stream to send the MouseEvent to.
fn send_mouse_event(
    movement: Position,
    phase: fidl_fuchsia_ui_input::PointerEventPhase,
    buttons: HashSet<MouseButton>,
    device_descriptor: &input_device::InputDeviceDescriptor,
    event_time: input_device::EventTime,
    sender: &mut Sender<input_device::InputEvent>,
) {
    // Only send move events when there are no buttons pressed.
    if phase != fidl_fuchsia_ui_input::PointerEventPhase::Move && buttons.is_empty() {
        return;
    }

    // Don't send move events when there is no movement.
    if phase == fidl_fuchsia_ui_input::PointerEventPhase::Move
        && movement.x == 0.0
        && movement.y == 0.0
    {
        return;
    }

    match sender.try_send(input_device::InputEvent {
        device_event: input_device::InputDeviceEvent::Mouse(MouseEvent::new(
            movement, phase, buttons,
        )),
        device_descriptor: device_descriptor.clone(),
        event_time,
    }) {
        Err(e) => fx_log_err!("Failed to send MouseEvent with error: {:?}", e),
        _ => {}
    }
}

/// Returns a u32 representation of `vector`, where each u8 of `vector` is an id of a button and
/// indicates the position of a bit to set.
///
/// This supports vectors with numbers from 1 - fidl_input_report::MOUSE_MAX_NUM_BUTTONS.
///
/// # Parameters
/// - `vector`: The vector containing the position of bits to be set.
///
/// # Example
/// ```
/// let bits = get_u32_from_vector(&vec![1, 3, 5]);
/// assert_eq!(bits, 21 /* ...00010101 */)
/// ```
pub fn get_u32_from_buttons(buttons: &HashSet<MouseButton>) -> u32 {
    let mut bits: u32 = 0;
    for button in buttons {
        if *button > 0 && *button <= fidl_input_report::MOUSE_MAX_NUM_BUTTONS as u8 {
            bits = ((1 as u32) << *button - 1) | bits;
        }
    }

    bits
}

/// Returns the set of pressed buttons present in the given input report.
///
/// # Parameters
/// - `report`: The input report to parse the mouse buttons from.
fn buttons_from_report(input_report: &fidl_input_report::InputReport) -> HashSet<MouseButton> {
    buttons_from_optional_report(&Some(input_report))
}

/// Returns the set of pressed buttons present in the given input report.
///
/// # Parameters
/// - `report`: The input report to parse the mouse buttons from.
fn buttons_from_optional_report(
    input_report: &Option<&fidl_input_report::InputReport>,
) -> HashSet<MouseButton> {
    input_report
        .as_ref()
        .and_then(|unwrapped_report| unwrapped_report.mouse.as_ref())
        .and_then(|mouse_report| match &mouse_report.pressed_buttons {
            Some(buttons) => Some(HashSet::from_iter(buttons.iter().cloned())),
            None => None,
        })
        .unwrap_or_default()
}

#[cfg(test)]
mod tests {
    use {super::*, crate::testing_utilities, fuchsia_async as fasync, futures::StreamExt};

    // Tests that the right u32 representation is returned from a vector of digits.
    #[test]
    fn get_u32_from_buttons_test() {
        let bits = get_u32_from_buttons(&HashSet::from_iter(vec![1, 3, 5].into_iter()));
        assert_eq!(bits, 21 /* 0...00010101 */)
    }

    // Tests that the right u32 representation is returned from a vector of digits that includes 0.
    #[test]
    fn get_u32_with_0_in_vector() {
        let bits = get_u32_from_buttons(&HashSet::from_iter(vec![0, 1, 3].into_iter()));
        assert_eq!(bits, 5 /* 0...00000101 */)
    }

    // Tests that the right u32 representation is returned from an empty vector.
    #[test]
    fn get_u32_with_empty_vector() {
        let bits = get_u32_from_buttons(&HashSet::new());
        assert_eq!(bits, 0 /* 0...00000000 */)
    }

    // Tests that the right u32 representation is returned from a vector containing std::u8::MAX.
    #[test]
    fn get_u32_with_u8_max_in_vector() {
        let bits = get_u32_from_buttons(&HashSet::from_iter(vec![1, 3, std::u8::MAX].into_iter()));
        assert_eq!(bits, 5 /* 0...00000101 */)
    }

    // Tests that the right u32 representation is returned from a vector containing the largest
    // button id possible.
    #[test]
    fn get_u32_with_max_mouse_buttons() {
        let bits = get_u32_from_buttons(&HashSet::from_iter(
            vec![1, 3, fidl_input_report::MOUSE_MAX_NUM_BUTTONS as MouseButton].into_iter(),
        ));
        assert_eq!(bits, 2147483653 /* 10...00000101 */)
    }

    /// Tests that a report containing no buttons but with movement generates a move event.
    #[fasync::run_singlethreaded(test)]
    async fn movement_without_button() {
        let movement = Position { x: 10.0, y: 16.0 };
        let (event_time_i64, event_time_u64) = testing_utilities::event_times();
        let first_report =
            testing_utilities::create_mouse_input_report(movement, vec![], event_time_i64);
        let descriptor =
            input_device::InputDeviceDescriptor::Mouse(MouseDeviceDescriptor { device_id: 1 });

        let input_reports = vec![first_report];
        let expected_events = vec![testing_utilities::create_mouse_event(
            movement,
            fidl_fuchsia_ui_input::PointerEventPhase::Move,
            HashSet::new(),
            event_time_u64,
            &descriptor,
        )];

        assert_input_report_sequence_generates_events!(
            input_reports: input_reports,
            expected_events: expected_events,
            device_descriptor: descriptor,
            device_type: MouseBinding,
        );
    }

    /// Tests that a report containing a new mouse button generates a down event.
    #[fasync::run_singlethreaded(test)]
    async fn down_without_movement() {
        let mouse_button: MouseButton = 3;
        let (event_time_i64, event_time_u64) = testing_utilities::event_times();
        let first_report = testing_utilities::create_mouse_input_report(
            Position { x: 0.0, y: 0.0 },
            vec![mouse_button],
            event_time_i64,
        );
        let descriptor =
            input_device::InputDeviceDescriptor::Mouse(MouseDeviceDescriptor { device_id: 1 });

        let input_reports = vec![first_report];
        let expected_events = vec![testing_utilities::create_mouse_event(
            Position { x: 0.0, y: 0.0 },
            fidl_fuchsia_ui_input::PointerEventPhase::Down,
            HashSet::from_iter(vec![mouse_button].into_iter()),
            event_time_u64,
            &descriptor,
        )];

        assert_input_report_sequence_generates_events!(
            input_reports: input_reports,
            expected_events: expected_events,
            device_descriptor: descriptor,
            device_type: MouseBinding,
        );
    }

    /// Tests that a report containing a new mouse button with movement generates a down event and a
    /// move event.
    #[fasync::run_singlethreaded(test)]
    async fn down_with_movement() {
        let movement = Position { x: 10.0, y: 16.0 };
        let mouse_button: MouseButton = 3;
        let (event_time_i64, event_time_u64) = testing_utilities::event_times();
        let first_report = testing_utilities::create_mouse_input_report(
            movement,
            vec![mouse_button],
            event_time_i64,
        );
        let descriptor =
            input_device::InputDeviceDescriptor::Mouse(MouseDeviceDescriptor { device_id: 1 });

        let input_reports = vec![first_report];
        let expected_events = vec![
            testing_utilities::create_mouse_event(
                Position { x: 0.0, y: 0.0 },
                fidl_fuchsia_ui_input::PointerEventPhase::Down,
                HashSet::from_iter(vec![mouse_button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
            testing_utilities::create_mouse_event(
                movement,
                fidl_fuchsia_ui_input::PointerEventPhase::Move,
                HashSet::from_iter(vec![mouse_button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
        ];

        assert_input_report_sequence_generates_events!(
            input_reports: input_reports,
            expected_events: expected_events,
            device_descriptor: descriptor,
            device_type: MouseBinding,
        );
    }

    /// Tests that a press and release of a mouse button without movement generates a down and up event.
    #[fasync::run_singlethreaded(test)]
    async fn down_up() {
        let button = 1;
        let (event_time_i64, event_time_u64) = testing_utilities::event_times();
        let first_report = testing_utilities::create_mouse_input_report(
            Position { x: 0.0, y: 0.0 },
            vec![button],
            event_time_i64,
        );
        let second_report = testing_utilities::create_mouse_input_report(
            Position { x: 0.0, y: 0.0 },
            vec![],
            event_time_i64,
        );
        let descriptor =
            input_device::InputDeviceDescriptor::Mouse(MouseDeviceDescriptor { device_id: 1 });

        let input_reports = vec![first_report, second_report];
        let expected_events = vec![
            testing_utilities::create_mouse_event(
                Position { x: 0.0, y: 0.0 },
                fidl_fuchsia_ui_input::PointerEventPhase::Down,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
            testing_utilities::create_mouse_event(
                Position { x: 0.0, y: 0.0 },
                fidl_fuchsia_ui_input::PointerEventPhase::Up,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
        ];

        assert_input_report_sequence_generates_events!(
            input_reports: input_reports,
            expected_events: expected_events,
            device_descriptor: descriptor,
            device_type: MouseBinding,
        );
    }

    /// Tests that a press and release of a mouse button with movement generates down, move, and up events.
    #[fasync::run_singlethreaded(test)]
    async fn down_up_with_movement() {
        let movement = Position { x: 10.0, y: 16.0 };
        let button = 1;

        let (event_time_i64, event_time_u64) = testing_utilities::event_times();
        let first_report = testing_utilities::create_mouse_input_report(
            Position { x: 0.0, y: 0.0 },
            vec![button],
            event_time_i64,
        );
        let second_report =
            testing_utilities::create_mouse_input_report(movement, vec![], event_time_i64);
        let descriptor =
            input_device::InputDeviceDescriptor::Mouse(MouseDeviceDescriptor { device_id: 1 });

        let input_reports = vec![first_report, second_report];
        let expected_events = vec![
            testing_utilities::create_mouse_event(
                Position { x: 0.0, y: 0.0 },
                fidl_fuchsia_ui_input::PointerEventPhase::Down,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
            testing_utilities::create_mouse_event(
                movement,
                fidl_fuchsia_ui_input::PointerEventPhase::Move,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
            testing_utilities::create_mouse_event(
                Position { x: 0.0, y: 0.0 },
                fidl_fuchsia_ui_input::PointerEventPhase::Up,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
        ];

        assert_input_report_sequence_generates_events!(
            input_reports: input_reports,
            expected_events: expected_events,
            device_descriptor: descriptor,
            device_type: MouseBinding,
        );
    }

    /// Tests that a press, move, and release of a button generates down, move, and up events.
    /// This specifically tests the separate input report containing the movement, instead of sending
    /// the movement as part of the down or up events.
    #[fasync::run_singlethreaded(test)]
    async fn down_move_up() {
        let movement = Position { x: 10.0, y: 16.0 };
        let button = 1;

        let (event_time_i64, event_time_u64) = testing_utilities::event_times();
        let first_report = testing_utilities::create_mouse_input_report(
            Position { x: 0.0, y: 0.0 },
            vec![button],
            event_time_i64,
        );
        let second_report =
            testing_utilities::create_mouse_input_report(movement, vec![button], event_time_i64);
        let third_report = testing_utilities::create_mouse_input_report(
            Position { x: 0.0, y: 0.0 },
            vec![],
            event_time_i64,
        );
        let descriptor =
            input_device::InputDeviceDescriptor::Mouse(MouseDeviceDescriptor { device_id: 1 });

        let input_reports = vec![first_report, second_report, third_report];
        let expected_events = vec![
            testing_utilities::create_mouse_event(
                Position { x: 0.0, y: 0.0 },
                fidl_fuchsia_ui_input::PointerEventPhase::Down,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
            testing_utilities::create_mouse_event(
                movement,
                fidl_fuchsia_ui_input::PointerEventPhase::Move,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
            testing_utilities::create_mouse_event(
                Position { x: 0.0, y: 0.0 },
                fidl_fuchsia_ui_input::PointerEventPhase::Up,
                HashSet::from_iter(vec![button].into_iter()),
                event_time_u64,
                &descriptor,
            ),
        ];

        assert_input_report_sequence_generates_events!(
            input_reports: input_reports,
            expected_events: expected_events,
            device_descriptor: descriptor,
            device_type: MouseBinding,
        );
    }
}
