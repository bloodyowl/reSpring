let unitOfTime = 1.0 /. 60.0;

let precision = 0.01;

let step = (~lastValue, ~lastVelocity, ~toValue, ~stiffness, ~damping) => {
  let spring = -. stiffness *. (lastValue -. toValue);
  let damper = -. damping *. lastVelocity;
  let all = spring +. damper;
  let nextVelocity = lastVelocity +. all *. unitOfTime;
  let nextValue = lastValue +. nextVelocity *. unitOfTime;
  let shouldRest =
    abs_float(nextVelocity) < precision
    && abs_float(nextValue -. toValue) < precision;
  (shouldRest ? toValue : nextValue, shouldRest ? 0.0 : nextVelocity);
};

type animationFrameTimerId;

[@bs.val]
external requestAnimationFrame : (float => unit) => animationFrameTimerId = "";

[@bs.val] external cancelAnimationFrame : animationFrameTimerId => unit = "";

type springCommands = {
  start: unit => unit,
  stop: unit => unit,
};

type t = (float, float);

let spring =
    (
      ~stiffness=180.0,
      ~damping=12.0,
      ~from,
      ~toValue,
      ~onChange,
      ~onRest=?,
      (),
    ) => {
  let animationFrameId = ref(None);
  let rec tick = ((currentValue, currentVelocity)) => {
    let (value, velocity) =
      step(
        ~lastValue=currentValue,
        ~lastVelocity=currentVelocity,
        ~toValue,
        ~stiffness,
        ~damping,
      );
    onChange((value, velocity));
    if (velocity == 0.0 || value == toValue) {
      switch (onRest) {
      | Some(func) => func()
      | None => ()
      };
    } else {
      animationFrameId :=
        Some(requestAnimationFrame((_) => tick((value, velocity))));
    };
  };
  {
    start: () =>
      animationFrameId := Some(requestAnimationFrame((_) => tick(from))),
    stop: () =>
      switch (animationFrameId^) {
      | Some(id) => cancelAnimationFrame(id)
      | None => ()
      },
  };
};

let parallel = springs =>
  Js.Promise.all(
    List.map(
      item =>
        Js.Promise.make((~resolve, ~reject as _reject) =>
          item(~onRest=() => resolve(. true), ()).start()
        ),
      springs,
    )
    |> Array.of_list,
  )
  |> Js.Promise.then_((_) => Js.Promise.resolve(true));

let rec sequence = list =>
  switch (list) {
  | [] => Js.Promise.resolve(true)
  | [head, ...tail] =>
    Js.Promise.make((~resolve, ~reject as _reject) =>
      head(~onRest=() => resolve(. true), ()).start()
    )
    |> Js.Promise.then_((_) => sequence(tail))
  };
