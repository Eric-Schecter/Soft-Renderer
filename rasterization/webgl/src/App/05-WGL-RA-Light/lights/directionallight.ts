import { vec3, vec4 } from "gl-matrix";
import { Light } from "./light";

export class DirectionalLight extends Light {
	constructor(
		public direction = vec3.create(),
		public color = vec4.create(),
		public intensity = 0
	){
		super();
	}
};