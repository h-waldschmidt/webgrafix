mod state;

use winit::{
    event::*,
    event_loop::{ControlFlow, EventLoop},
    window::WindowBuilder,
};

fn main() {
    env_logger::init(); // Necessary for logging within WGPU
    let event_loop = EventLoop::new(); // Loop provided by winit for handling window events
    let window = WindowBuilder::new().build(&event_loop).unwrap();

    // Opens the window and starts processing events (although no events are handled yet)
    event_loop.run(move |event, _, control_flow: &mut ControlFlow| {
        *control_flow = ControlFlow::Wait;

        match event {
            Event::WindowEvent {
                event: WindowEvent::CloseRequested,
                window_id,
            } if window_id == window.id() => *control_flow = ControlFlow::Exit,
            Event::WindowEvent {
                event: WindowEvent::KeyboardInput { input, .. },
                window_id,
            } if window_id == window.id() => {
                if input.virtual_keycode == Some(VirtualKeyCode::Escape) {
                    *control_flow = ControlFlow::Exit
                }
            }
            _ => (),
        }
    });
}
