import TgaLoader from 'tga-js';

export class Texture {
  public id: WebGLTexture = 0;
  public location : WebGLUniformLocation = 0;
  constructor(private gl: WebGL2RenderingContext) { }
  public load = (path: string) => {
    const tgaLoader = new TgaLoader();
    return new Promise((resolve, reject) => {
      tgaLoader.open(path, () => {
        const {data,width,height} = tgaLoader.getImageData();
        // create + bind
        this.id = this.gl.createTexture() as WebGLTexture;
        this.gl.bindTexture(this.gl.TEXTURE_2D, this.id);
        // set parameters
        this.gl.pixelStorei(this.gl.UNPACK_FLIP_Y_WEBGL,true);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_S, this.gl.REPEAT);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_WRAP_T, this.gl.REPEAT);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MIN_FILTER, this.gl.LINEAR);
        this.gl.texParameteri(this.gl.TEXTURE_2D, this.gl.TEXTURE_MAG_FILTER, this.gl.LINEAR);
        // set data
        this.gl.texStorage2D(this.gl.TEXTURE_2D, 1, this.gl.RGBA8, width, height);
        this.gl.texSubImage2D(this.gl.TEXTURE_2D, 0, 0, 0, width, height, this.gl.RGBA, this.gl.UNSIGNED_BYTE, data);
        // mipmap
        this.gl.generateMipmap(this.gl.TEXTURE_2D);
        resolve(true);
      })
    })
  }
}