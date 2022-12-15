import { Application } from "../gl";
import vs from './shader/triangle.vs';
import fs from './shader/triangle.fs';
import { Mesh } from "./mesh";
import { Camera } from "./camera";
import { vec3 } from "gl-matrix";

export class CameraDemo extends Application {
  private program: WebGLProgram;
  private mesh: Mesh;
  private camera: Camera;
  constructor(container: HTMLDivElement) {
    super(container);
    const { clientWidth, clientHeight } = this.canvas;
    this.program = this.programLoader.load(this.gl, vs, fs);
    this.mesh = new Mesh(this.gl);

    const fov = 60 / 180 * Math.PI;
    const aspect = clientWidth / clientHeight;
    const near = 0.1;
    const far = 1000;
    this.camera = new Camera();
    this.camera.pos = vec3.fromValues(1, 1, 2);
    this.camera.setProjection(fov, aspect, near, far);
    this.camera.setView(vec3.fromValues(0, 0, 0));
    this.camera.setViewport(0, 0, clientWidth, clientHeight);

    this.gl.enable(this.gl.DEPTH_TEST);
  }
  private setupUniforms = () => {
    const uViewMatrix = this.gl.getUniformLocation(this.program, "uViewMatrix");
    const uProjectionMatrix = this.gl.getUniformLocation(this.program, "uProjectionMatrix");
    const uModelMatrix = this.gl.getUniformLocation(this.program, "uModelMatrix");

    this.gl.uniformMatrix4fv(uProjectionMatrix, false, this.camera.projection);
    this.gl.uniformMatrix4fv(uViewMatrix, false, this.camera.view);
    this.gl.uniformMatrix4fv(uModelMatrix, false, this.mesh.modelMatrix);
  }
  public setup = async () => {
    await this.mesh.loadModel('models/african_head/african_head.obj');
    this.gl.useProgram(this.program);
    this.setupUniforms();
    this.gl.useProgram(null);
  }
  protected update = (time: number) => {
    const viewport = this.camera.viewport;
    this.gl.viewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    this.gl.clearColor(0, 0, 0, 1);
    this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);

    this.gl.useProgram(this.program);
    this.gl.bindVertexArray(this.mesh.vao);
    this.gl.drawElements(this.gl.TRIANGLES, this.mesh.indices.length, this.gl.UNSIGNED_SHORT, 0);
    this.gl.bindVertexArray(null);
    this.gl.useProgram(null);
  }
}