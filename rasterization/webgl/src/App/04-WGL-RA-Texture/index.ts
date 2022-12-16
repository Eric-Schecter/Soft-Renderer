import { Application } from "../gl";
import vs from './shader/triangle.vs';
import fs from './shader/triangle.fs';
import { Mesh } from "./mesh";
import { Camera } from "./camera";
import { vec3 } from "gl-matrix";

export class Texture extends Application {
  private mesh: Mesh;
  private camera: Camera;
  constructor(container: HTMLDivElement) {
    super(container);
    const { clientWidth, clientHeight } = this.canvas;
    this.mesh = new Mesh(this.gl,vs,fs);

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
  public setup = async () => {
    await Promise.all([
      this.mesh.loadModel('models/african_head/african_head.obj'),
      this.mesh.loadTexture('models/african_head/african_head_diffuse.tga','diffuseMap'),      
      this.mesh.loadTexture('models/african_head/african_head_nm_tangent.tga','normalMap')
    ])
    this.mesh.setupUniforms(this.camera);
  }
  protected update = (time: number) => {
    const viewport = this.camera.viewport;
    this.gl.viewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    this.gl.clearColor(0, 0, 0, 1);
    this.gl.clear(this.gl.COLOR_BUFFER_BIT | this.gl.DEPTH_BUFFER_BIT);

    this.gl.useProgram(this.mesh.program);
    this.gl.bindVertexArray(this.mesh.vao);
    this.gl.drawElements(this.gl.TRIANGLES, this.mesh.indices.length, this.gl.UNSIGNED_SHORT, 0);
    this.gl.bindVertexArray(null);
    this.gl.useProgram(null);
  }
}