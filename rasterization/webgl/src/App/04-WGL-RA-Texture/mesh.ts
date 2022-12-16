import { OBJ } from 'webgl-obj-loader';
import { mat4 } from 'gl-matrix';
import { Texture } from './texture';
import { Camera } from './camera';
import { ProgramLoader } from '../gl/programLoader';

export class Mesh {
  private static programLoader = new ProgramLoader();
  public program: WebGLProgram;
  public vao!: WebGLVertexArrayObject;
  public vertices: number[] = [];
  public indices: number[] = [];
  public textures: Texture[] = [];
  public modelMatrix = mat4.create();
  constructor(private gl: WebGL2RenderingContext, vs: string, fs: string) {
    this.program = Mesh.programLoader.load(this.gl, vs, fs);
  }
  public setupUniforms = (camera: Camera) => {
    this.gl.useProgram(this.program);

    const uViewMatrix = this.gl.getUniformLocation(this.program, "uViewMatrix");
    const uProjectionMatrix = this.gl.getUniformLocation(this.program, "uProjectionMatrix");
    const uModelMatrix = this.gl.getUniformLocation(this.program, "uModelMatrix");
    const uNormalMatrix = this.gl.getUniformLocation(this.program, "uNormalMatrix");

    this.gl.uniformMatrix4fv(uProjectionMatrix, false, camera.projection);
    this.gl.uniformMatrix4fv(uViewMatrix, false, camera.view);
    this.gl.uniformMatrix4fv(uModelMatrix, false, this.modelMatrix);
    const normalMatrix = mat4.transpose(mat4.create(), (mat4.invert(mat4.create(), this.modelMatrix)));
    this.gl.uniformMatrix4fv(uNormalMatrix, false, normalMatrix);

    this.textures.forEach(({ id, location }, index) => {
      this.gl.activeTexture(this.gl.TEXTURE0 + index);
      this.gl.bindTexture(this.gl.TEXTURE_2D, id);
      this.gl.uniform1i(location, index);
    })

    this.gl.useProgram(null);
  }
  public loadModel = async (modelPath: string) => {
    const res = await fetch(modelPath);
    const text = await res.text();

    const mesh = new OBJ.Mesh(text, { calcTangentsAndBitangents: true });
    let i = 0;
    let j = 0;
    while (i < mesh.vertices.length) {
      this.vertices.push(mesh.vertices[i], mesh.vertices[i + 1], mesh.vertices[i + 2]);
      this.vertices.push(mesh.vertexNormals[i], mesh.vertexNormals[i + 1], mesh.vertexNormals[i + 2]);
      this.vertices.push(mesh.textures[j], mesh.textures[j + 1]);
      this.vertices.push(mesh.tangents[i], mesh.tangents[i + 1], mesh.tangents[i + 2]);
      this.vertices.push(mesh.bitangents[i], mesh.bitangents[i + 1], mesh.bitangents[i + 2]);
      i += 3;
      j += 2;
    }
    this.indices = mesh.indices;

    // init
    this.vao = this.gl.createVertexArray() as WebGLVertexArrayObject;
    const vbo = this.gl.createBuffer();
    const ibo = this.gl.createBuffer();
    // bind
    this.gl.bindVertexArray(this.vao);
    this.gl.bindBuffer(this.gl.ARRAY_BUFFER, vbo);
    this.gl.bindBuffer(this.gl.ELEMENT_ARRAY_BUFFER, ibo);
    // pass data to buffer
    this.gl.bufferData(this.gl.ARRAY_BUFFER, new Float32Array(this.vertices), this.gl.STATIC_DRAW);
    this.gl.bufferData(this.gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(this.indices), this.gl.STATIC_DRAW);
    // bind buffer to variable in gpu
    this.gl.vertexAttribPointer(0, 3, this.gl.FLOAT, false, 14 * 4, 0);
    this.gl.vertexAttribPointer(1, 3, this.gl.FLOAT, false, 14 * 4, 3 * 4);
    this.gl.vertexAttribPointer(2, 2, this.gl.FLOAT, false, 14 * 4, 6 * 4);
    this.gl.vertexAttribPointer(3, 3, this.gl.FLOAT, false, 14 * 4, 8 * 4);
    this.gl.vertexAttribPointer(4, 3, this.gl.FLOAT, false, 14 * 4, 11 * 4);
    // enable varialbe
    this.gl.enableVertexAttribArray(0);
    this.gl.enableVertexAttribArray(1);
    this.gl.enableVertexAttribArray(2);
    this.gl.enableVertexAttribArray(3);
    this.gl.enableVertexAttribArray(4);
    // reset
    this.gl.bindVertexArray(null);
  }
  public loadTexture = async (path: string, name: string) => {
    const texture = new Texture(this.gl);
    texture.location = this.gl.getUniformLocation(this.program, name) as WebGLUniformLocation;
    await texture.load(path);
    this.textures.push(texture);
  }
}