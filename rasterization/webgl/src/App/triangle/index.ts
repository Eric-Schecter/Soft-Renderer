import { Application } from "../gl";
import vs from './shader/triangle.vs';
import fs from './shader/triangle.fs';

export class Triangle extends Application {
  private vao: WebGLVertexArrayObject;
  private program: WebGLProgram;
  private data = [
    -0.5, -0.5, 0.0, 1.0, 0.0, 0.0,
    0.5, -0.5, 0.0, 0.0, 1.0, 0.0,
    0.0, 0.5, 0.0, 0.0, 0.0, 1.0
  ];
  constructor(container: HTMLDivElement) {
    super(container);
    this.program = this.programLoader.load(this.gl, vs, fs);

    // init
    this.vao = this.gl.createVertexArray() as WebGLVertexArrayObject;
    const vbo = this.gl.createBuffer();
    // bind
    this.gl.bindVertexArray(this.vao);
    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, vbo);
    // pass data to buffer
    this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(this.data), this.gl.STATIC_DRAW);
    // bind buffer to variable in gpu
    this.gl.vertexAttribPointer(0, 3, this.gl.FLOAT, false, 24, 0);
    this.gl.vertexAttribPointer(1, 3, this.gl.FLOAT, false, 24, 12);
    // enable varialbe
    this.gl.enableVertexAttribArray(0);
    this.gl.enableVertexAttribArray(1);
    // reset
    this.gl.bindVertexArray(null);
  }
  protected update = (time: number) => {
    const { clientWidth, clientHeight } = this.canvas;
    this.gl.viewport(0, 0, clientWidth, clientHeight);

    this.gl.clearColor(0, 0, 0, 1);
    this.gl.clear(this.gl.COLOR_BUFFER_BIT);

    this.gl.useProgram(this.program);
    this.gl.bindVertexArray(this.vao);
    this.gl.drawArrays(this.gl.TRIANGLES, 0, 3);
    this.gl.bindVertexArray(null);
    this.gl.useProgram(null);
  }
}