import React from 'react';
import ReactDOM from 'react-dom';
import './index.module.scss';
import * as serviceWorker from './serviceWorker';
import { App } from './App';
import { HashRouter, Route, Switch } from 'react-router-dom';
import { TriangleDemo } from './App/01-WGL-RA-Triangle';
import { SimpleModelDemo } from './App/02-WGL-RA-SimpleModel';
import { CameraDemo } from './App/03-WGL-RA-Camera';
import { TextureDemo } from './App/04-WGL-RA-Texture';
import { LightDemo } from './App/05-WGL-RA-Light';

const routes = [
  { path: '/', app: TriangleDemo },
  { path: '/triangle', app: TriangleDemo },
  { path: '/simplemodel', app: SimpleModelDemo },
  { path: '/camera', app: CameraDemo },
  { path: '/texture', app: TextureDemo },
  { path: '/light', app: LightDemo },
]

ReactDOM.render(
  <React.StrictMode>
    <HashRouter>
      <Switch>
        {routes.map((d, i) => <Route key={d.path + i} exact path={d.path} render={() => <App GL_App={d.app} />} />)}
      </Switch>
    </HashRouter>
  </React.StrictMode>,
  document.getElementById('root')
);

serviceWorker.register();
