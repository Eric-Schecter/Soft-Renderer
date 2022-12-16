export abstract class Light {}

export type LightType = 
| 'AmbientLight'
| 'DirectionalLight'
| 'SpotLight'
| 'PointLight'