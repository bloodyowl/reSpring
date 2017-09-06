let unitOfTime = 1.0 /. 60.0;

let precision = 0.01;

let step ::lastValue ::lastVelocity ::toValue ::stiffness ::damping => {
  let spring = -. stiffness *. (lastValue -. toValue);
  let damper = -. damping *. lastVelocity;
  let all = spring +. damper;
  let nextVelocity = lastVelocity +. all *. unitOfTime;
  let nextValue = lastValue +. nextVelocity *. unitOfTime;
  let shouldRest =
    abs_float nextVelocity < precision && abs_float (nextValue -. toValue) < precision;
  (shouldRest ? toValue : nextValue, shouldRest ? 0.0 : nextVelocity)
};

type animationFrameTimerId;

external requestAnimationFrame : (float => unit) => animationFrameTimerId = "" [@@bs.val];

external cancelAnimationFrame : animationFrameTimerId => unit = "" [@@bs.val];

type springCommands = {start: unit => unit, stop: unit => unit};

type t = (float, float);

let spring ::stiffness=180.0 ::damping=12.0 ::from ::toValue ::onChange ::onRest=? () => {
  let animationFrameId = ref None;
  let rec tick (currentValue, currentVelocity) => {
    let (value, velocity) =
      step lastValue::currentValue lastVelocity::currentVelocity ::toValue ::stiffness ::damping;
    onChange (value, velocity);
    if (velocity == 0.0 || value == toValue) {
      switch onRest {
      | Some func => func ()
      | None => ()
      }
    } else {
      animationFrameId := Some (requestAnimationFrame (fun _ => tick (value, velocity)))
    }
  };
  {
    start: fun () => animationFrameId := Some (requestAnimationFrame (fun _ => tick from)),
    stop: fun () =>
      switch !animationFrameId {
      | Some id => cancelAnimationFrame id
      | None => ()
      }
  }
};

let parallel springs =>
  Js.Promise.all (
    List.map
      (
        fun item =>
          Js.Promise.make (
            fun ::resolve reject::_reject =>
              (item onRest::(fun () => resolve true [@bs]) ()).start ()
          )
      )
      springs |> Array.of_list
  ) |>
  Js.Promise.then_ (fun _ => Js.Promise.resolve true);

let rec sequence list =>
  switch list {
  | [] => Js.Promise.resolve true
  | [head, ...tail] =>
    Js.Promise.make (
      fun ::resolve reject::_reject => (head onRest::(fun () => resolve true [@bs]) ()).start ()
    ) |>
    Js.Promise.then_ (fun _ => sequence tail)
  };
