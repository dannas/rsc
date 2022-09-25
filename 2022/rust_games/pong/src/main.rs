use raylib::prelude::*;

#[derive(Debug)]
struct Ball {
    p: Vector2,
    v: Vector2,
    r: f32,
    active: bool,
    serve_delay: i32,
}

#[derive(Debug)]
struct Player {
    p: Vector2,
    sz: Vector2,
    points: i32,
}

fn main() {
    const W: f32 = 640.0;
    const H: f32 = 480.0;
    let (mut rl, thread) = raylib::init()
        .size(W as i32, H as i32)
        .title("Ping pong")
        .build();

    rl.set_target_fps(60);

    let mut ball = Ball {
        r: 7.0,
        p: Vector2 {
            x: W / 2.0,
            y: H / 2.0,
        },
        v: Vector2 { x: -7.0, y: -2.0 },
        active: false,
        serve_delay: 100,
    };

    let mut player = Player {
        p: Vector2 {
            x: W - 20.0,
            y: H / 2.0,
        },
        sz: Vector2 {
            x: 10.0,
            y: H / 10.0,
        },
        points: 0,
    };

    let mut opponent = Player {
        p: Vector2 {
            x: 20.0,
            y: H / 2.0,
        },
        sz: Vector2 {
            x: 10.0,
            y: H / 10.0,
        },
        points: 0,
    };

    while !rl.window_should_close() {
        // input
        {
            if rl.is_key_down(KeyboardKey::KEY_UP) {
                player.p.y -= 6.0;
            }
            if rl.is_key_down(KeyboardKey::KEY_DOWN) {
                player.p.y += 6.0;
            }
        }
        // update
        {
            // AI: Move opponent towards the ball when the ball is moving towards him.
            if ball.v.x < 0.0 && opponent.p.y < ball.p.y {
                opponent.p.y += 2.0;
            }
            if ball.v.x < 0.0 && opponent.p.y > ball.p.y {
                opponent.p.y -= 2.0;
            }

            // Move ball
            if ball.active {
                ball.p += ball.v;
            } else {
                ball.serve_delay -= 1;
                if ball.serve_delay <= 0 {
                    ball.active = true;
                    ball.serve_delay =  100;
                }
            }

            // Check player collision with walls
            if player.p.y - player.sz.y / 2.0 <= 0.0 {
                player.p.y = player.sz.y / 2.0;
            }
            if player.p.y + player.sz.y / 2.0 >= H {
                player.p.y = H - player.sz.y / 2.0;
            }

            // Check ball collisions with player
            let r = Rectangle::new(
                player.p.x - player.sz.x / 2.0,
                player.p.y - player.sz.y / 2.0,
                player.sz.x,
                player.sz.y,
            );

            if r.check_collision_circle_rec(ball.p, ball.r) {
                if ball.v.x > 0.0 {
                    ball.v.x *= -1.0;
                }
                ball.v.y = (player.p.y - ball.p.y) / player.sz.y * 5.0;
                ball.p += ball.v;
            }

            // Check ball collisions with opponent
            let r = Rectangle::new(
                opponent.p.x - opponent.sz.x / 2.0,
                opponent.p.y - opponent.sz.y / 2.0,
                opponent.sz.x,
                opponent.sz.y,
            );

            if r.check_collision_circle_rec(ball.p, ball.r) {
                if ball.v.x < 0.0 {
                    ball.v.x *= -1.0;
                }
                ball.p += ball.v;
            }
            // Check if ball leaves table
            if ball.p.x - ball.r >= W {
                opponent.points += 1;
                ball.p = Vector2::new(W / 2.0, H / 2.0);
                ball.v = Vector2::new(-7.0, 0.0);
                ball.active = false;
                opponent.p.y = H / 2.0;
                player.p.y = H / 2.0;
            }
            if ball.p.x + ball.r <= 0.0 {
                player.points += 1;
                ball.p = Vector2::new(W / 2.0, H / 2.0);
                ball.v = Vector2::new(-7.0, 0.0);
                ball.active = false;
                opponent.p.y = H / 2.0;
                player.p.y = H / 2.0;
            }

            // Check ball collisions with wall
            if ball.p.y + ball.r >= H || ball.p.y - ball.r <= 0.0 {
                ball.v.y *= -1.0;
            }
        }
        // render
        {
            let mut d = rl.begin_drawing(&thread);
            d.clear_background(Color::RAYWHITE);

            for y in (0..H as i32).step_by(40) {
                d.draw_rectangle(W as i32 / 2, y, 10, 20, Color::LIGHTGRAY);
            }
            d.draw_text(&format!("{}", opponent.points), 10, 20, 50, Color::PURPLE);
            d.draw_text(&format!("{}", player.points), W as i32  - 50, 20, 50, Color::PURPLE);

            d.draw_circle_v(ball.p, ball.r, Color::BLACK);
            d.draw_rectangle_v(player.p - player.sz / 2.0, player.sz, Color::GRAY);
            d.draw_rectangle_v(opponent.p - opponent.sz / 2.0, opponent.sz, Color::GRAY);
        }
    }
}
