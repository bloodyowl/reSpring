type springCommands = {start: unit => unit, stop: unit => unit};

type t = (float, float);

let spring:
  stiffness::float? =>
  damping::float? =>
  from::(float, float) =>
  toValue::float =>
  onChange::(t => 'a) =>
  onRest::(unit => unit)? =>
  unit =>
  springCommands;


/**
 * Spring.(
 *   parallel [
 *     spring from::0 to::10 onChange::(reduce (fun (value, _) => MoveItem value)),
 *     spring from::10 to::0 onChange::(reduce (fun (value, _) => MoveOtherItem value))
 *   ] |> Js.Promise.then (fun _ => {
 *      Js.log "ok";
 *      Js.Promise.resolve ()
 *   })
 * )
 */
let parallel: list (onRest::(unit => unit) => unit => springCommands) => Js.Promise.t bool;


/**
 * Spring.(
 *   sequence [
 *     spring from::0 to::10 onChange::(reduce (fun (value, _) => MoveItem value)),
 *     spring from::10 to::0 onChange::(reduce (fun (value, _) => MoveItem value))
 *   ] |> Js.Promise.then (fun _ => {
 *      Js.log "ok";
 *      Js.Promise.resolve ()
 *   })
 * )
 */
let sequence: list (onRest::(unit => unit) => unit => springCommands) => Js.Promise.t bool;
