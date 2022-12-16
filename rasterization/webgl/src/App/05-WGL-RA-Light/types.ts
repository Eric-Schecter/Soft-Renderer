import { AmbientLight, DirectionalLight, PointLight, SpotLight } from "./lights";

export type Lights = {
  'AmbientLight':AmbientLight[],
  'DirectionalLight':DirectionalLight[],
  'PointLight':PointLight[],
  'SpotLight':SpotLight[],
}