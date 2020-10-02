import { lightTheme, darkTheme } from '~/renderer/constants/themes';

export const getTheme = (name: string) => {
  if (name === 'midori-light') return lightTheme;
  else if (name === 'midori-dark') return darkTheme;
  return lightTheme;
};
