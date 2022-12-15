import { Application } from "../gl";
import vs from './shader/triangle.vs';
import fs from './shader/triangle.fs';
import { Mesh } from "./mesh";

export class SimpleModel extends Application {
  private program: WebGLProgram;
  private mesh: Mesh;
  constructor(container: HTMLDivElement) {
    super(container);
    this.program = this.programLoader.load(this.gl, vs, fs);
    this.mesh = new Mesh(this.gl);

    this.gl.enable(this.gl.DEPTH_TEST);
  }
  public setup = async () => {
    await this.mesh.loadModel('models/african_head/african_head.obj');
  }
  protected update = (time: number) => {
    const { clientWidth, clientHeight } = this.canvas;
    this.gl.viewport(0, 0, clientWidth, clientHeight);

    this.gl.clearColor(0, 0, 0, 1);
    this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);

    this.gl.useProgram(this.program);
    this.gl.bindVertexArray(this.mesh.vao);
    this.gl.drawElements(this.gl.TRIANGLES, this.mesh.indices.length, this.gl.UNSIGNED_SHORT, 0);
    this.gl.bindVertexArray(null);
    this.gl.useProgram(null);
  }
}