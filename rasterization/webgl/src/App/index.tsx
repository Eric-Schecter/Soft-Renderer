import React, { useRef, useEffect } from 'react';
import styles from './index.module.scss';
import { Triangle } from './triangle';

export const App = () => {
  const ref = useRef<HTMLDivElement>(null);
  useEffect(() => {
    if (!ref.current) { return }
    const app = new Triangle(ref.current);
    try {
      app.run();
    } catch (error) {
      console.log(error);
    }
    return ()=>app.cleanup();
  }, [ref])

  return <div
    ref={ref}
    className={styles.main}
  />
}