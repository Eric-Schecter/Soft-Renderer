import { vec3, vec4 } from "gl-matrix";
import { Light } from "./light";

export class PointLight extends Light {
	constructor(
	  public color = vec4.create(),
		public pos = vec3.create(),
		public constant = 0,
		public intensity = 0,
		public linear = 0,
		public quadratic = 0
	){
		super();
	}
};