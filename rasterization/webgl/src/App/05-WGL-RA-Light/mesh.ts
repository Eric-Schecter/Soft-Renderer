import { OBJ } from 'webgl-obj-loader';
import { mat4 } from 'gl-matrix';
import { Texture } from './texture';
import { Camera } from './camera';
import { ProgramLoader } from '../gl/programLoader';
import { Material } from './material';
import { Lights } from './types';

export class Mesh {
  private static programLoader = new ProgramLoader();
  public program: WebGLProgram;
  public vao!: WebGLVertexArrayObject;
  public vertices: number[] = [];
  public indices: number[] = [];
  public textures: Texture[] = [];
  public modelMatrix = mat4.create();
  public material: Material = new Material();
  private uniformsTable: { [prop: string]: WebGLUniformLocation } = {};
  constructor(private gl: WebGL2RenderingContext, vs: string, fs: string) {
    this.program = Mesh.programLoader.load(this.gl, vs, fs);
  }
  public setupUniforms = (camera: Camera, lights: Lights) => {
    this.gl.useProgram(this.program);

    const count = this.gl.getProgramParameter(this.program, this.gl.ACTIVE_UNIFORMS);
    for (let i = 0; i < count; i++) {
      const info = this.gl.getActiveUniform(this.program, i);
      if (!info) { continue; }
      const { name } = info;
      const addr = this.gl.getUniformLocation(this.program, name);
      if (!addr) { continue; }
      this.uniformsTable[name] = addr;
    }

    const uViewMatrix = this.uniformsTable['uViewMatrix'];
    uViewMatrix && this.gl.uniformMatrix4fv(uViewMatrix, false, camera.view);

    const uProjectionMatrix = this.uniformsTable['uProjectionMatrix'];
    uProjectionMatrix && this.gl.uniformMatrix4fv(uProjectionMatrix, false, camera.projection);

    const uModelMatrix = this.uniformsTable['uModelMatrix'];
    uModelMatrix && this.gl.uniformMatrix4fv(uModelMatrix, false, this.modelMatrix);

    const uNormalMatrix = this.uniformsTable['uNormalMatrix'];
    if (uNormalMatrix) {
      const normalMatrix = mat4.transpose(mat4.create(), (mat4.invert(mat4.create(), this.modelMatrix)));
      this.gl.uniformMatrix4fv(uNormalMatrix, false, normalMatrix);
    }

    const specular = this.uniformsTable['uMaterial.specular'];
    specular && this.gl.uniform1f(specular, this.material.specular);

    const shininess = this.uniformsTable['uMaterial.shininess'];
    shininess && this.gl.uniform1f(shininess, this.material.shininess);

    const uCameraPos = this.uniformsTable['uCameraPos'];
    uCameraPos && this.gl.uniform3fv(uCameraPos, camera.pos);

    for (let i = 0; i < lights['AmbientLight'].length; i++) {
      const uAmbientLightColor = this.uniformsTable[`uAmbientLight[${i}].color`];
      uAmbientLightColor && this.gl.uniform4fv(uAmbientLightColor, lights['AmbientLight'][i].color);

      const uAmbientLightIntensity = this.uniformsTable[`uAmbientLight[${i}].intensity`];
      uAmbientLightIntensity && this.gl.uniform1f(uAmbientLightIntensity, lights['AmbientLight'][i].intensity);
    }

    for (let i = 0; i < lights['DirectionalLight'].length; i++) {
      const direction = this.uniformsTable[`uDirectionalLight[${i}].direction`];
      direction && this.gl.uniform3fv(direction, lights['DirectionalLight'][i].direction);

      const color = this.uniformsTable[`uDirectionalLight[${i}].color`];
      color && this.gl.uniform4fv(color, lights['DirectionalLight'][i].color);

      const intensity = this.uniformsTable[`uDirectionalLight[${i}].intensity`];
      intensity && this.gl.uniform1f(intensity, lights['DirectionalLight'][i].intensity);
    }

    for (let i = 0; i < lights['SpotLight'].length; i++) {
      const pos = this.uniformsTable[`uSpotLight[${i}].pos`];
      pos && this.gl.uniform3fv(pos, lights['SpotLight'][i].pos);

      const direction = this.uniformsTable[`uSpotLight[${i}].direction`];
      direction && this.gl.uniform3fv(direction, lights['SpotLight'][i].direction);

      const cutOff = this.uniformsTable[`uSpotLight[${i}].cutOff`];
      cutOff && this.gl.uniform1f(cutOff, lights['SpotLight'][i].cutOff);

      const outerCutOff = this.uniformsTable[`uSpotLight[${i}].outerCutOff`];
      outerCutOff && this.gl.uniform1f(outerCutOff, lights['SpotLight'][i].outerCutOff);

      const color = this.uniformsTable[`uSpotLight[${i}].color`];
      color && this.gl.uniform4fv(color, lights['SpotLight'][i].color);

      const intensity = this.uniformsTable[`uSpotLight[${i}].intensity`];
      intensity && this.gl.uniform1f(intensity, lights['SpotLight'][i].intensity);
    }

    for (let i = 0; i < lights['PointLight'].length; i++) {
      const pos = this.uniformsTable[`uPointLight[${i}].pos`];
      pos && this.gl.uniform3fv(pos, lights['PointLight'][i].pos);

      const color = this.uniformsTable[`uPointLight[${i}].color`];
      color && this.gl.uniform4fv(color, lights['PointLight'][i].color);

      const intensity = this.uniformsTable[`uPointLight[${i}].intensity`];
      intensity && this.gl.uniform1f(intensity, lights['SpotLight'][i].intensity);

      const constant = this.uniformsTable[`uPointLight[${i}].constant`];
      constant && this.gl.uniform1f(constant, lights['PointLight'][i].constant);

      const linear = this.uniformsTable[`uPointLight[${i}].linear`];
      linear && this.gl.uniform1f(linear, lights['PointLight'][i].linear);

      const quadratic = this.uniformsTable[`uPointLight[${i}].quadratic`];
      quadratic && this.gl.uniform1f(quadratic, lights['PointLight'][i].quadratic);
    }

    this.textures.forEach(({ id, location }, index) => {
      this.gl.activeTexture(this.gl.TEXTURE0 + index);
      this.gl.bindTexture(this.gl.TEXTURE_2D, id);
      this.gl.uniform1i(location, index);
    });

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