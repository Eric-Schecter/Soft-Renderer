import { vec4 } from "gl-matrix";
import { Light } from "./light";

export class AmbientLight extends Light {
	constructor(
		public color = vec4.create(),
		public intensity = 0
	){
		super();
	}
};