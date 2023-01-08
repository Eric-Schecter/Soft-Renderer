import { vec3, vec4 } from "gl-matrix";
import { Light } from "./light";

export class SpotLight extends Light {
	constructor(
		public color = vec4.create(),
		public pos = vec3.create(),
		public direction = vec3.create(),
		public cutOff = 0,
		public outerCutOff = 0,
		public intensity = 0,
	) {
		super();
	}
};