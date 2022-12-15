import React from 'react';
import ReactDOM from 'react-dom';
import './index.module.scss';
import * as serviceWorker from './serviceWorker';
import { App } from './App';
import { HashRouter, Route, Switch } from 'react-router-dom';
import { Triangle } from './App/01-WGL-RA-Triangle';
import { SimpleModel } from './App/02-WGL-RA-SimpleModel';
import { CameraDemo } from './App/03-WGL-RA-Camera';

const routes = [
  { path: '/', app: Triangle },
  { path: '/triangle', app: Triangle },
  { path: '/simplemodel', app: SimpleModel },
  { path: '/camera', app: CameraDemo },
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
