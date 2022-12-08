import React, { useRef, useEffect } from 'react';
import { Application } from './gl';
import styles from './index.module.scss';

type Props = {
  GL_App:new (container:HTMLDivElement) => Application
}

export const App = ({GL_App}:Props) => {
  const ref = useRef<HTMLDivElement>(null);
  useEffect(() => {
    if (!ref.current) { return }
    const app = new GL_App(ref.current);
    try {
      app.run();
    } catch (error) {
      console.log(error);
    }
    return ()=>app.cleanup();
  }, [ref,GL_App])

  return <div
    ref={ref}
    className={styles.main}
  />
}