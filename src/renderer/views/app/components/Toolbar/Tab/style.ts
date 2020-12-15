import styled, { css } from 'styled-components';

import { transparency, icons } from '~/renderer/constants';
import { ITheme } from '~/interfaces';
import { centerIcon, body2 } from '~/renderer/mixins';
import { TAB_PINNED_WIDTH } from '../../../constants';

interface CloseProps {
  visible: boolean;
  theme?: ITheme;
}

export const StyledClose = styled.div`
height: 20px;
width: 20px;
margin-left: 2px;
margin-right: 6px;
  background-image: url('${icons.close}');
  transition: 0.1s background-color;
  z-index: 10;
  ${centerIcon(16)};

    ${({ visible, theme }: CloseProps) => css`
      opacity: ${visible ? transparency.icons.inactive : 0};
      display: ${visible ? 'block' : 'none'};
      filter: ${theme['toolbar.lightForeground'] ? 'invert(100%)' : 'none'};
    `}

  &:hover {
    background-color: rgba(0, 0, 0, 0.1);
  }
`;

interface TabProps {
  selected: boolean;
  visible?: boolean;
}

export const StyledTab = styled.div`
  position: absolute;
  height: 100%;
  width: 0;
  left: 0;
  align-items: center;
  will-change: width;
  -webkit-app-region: no-drag;
  display: flex;

  ${({ selected, visible }: TabProps) => css`
    z-index: ${selected ? 2 : 1};
  `};
`;

export const StyledOverlay = styled.div`
  position: absolute;
  top: 0;
  left: 0;
  right: 0;
  bottom: 0;
  transition: 0.1s opacity;
  ${({ hovered }: { hovered: boolean }) => css`
    opacity: ${hovered ? 0.08 : 0};
  `};
`;

interface TitleProps {
  isIcon: boolean;
  selected: boolean;
  theme?: ITheme;
}

export const StyledTitle = styled.div`
  ${body2()};
  font-size: 12px;
  overflow: hidden;
  text-overflow: ellipsis;
  white-space: nowrap;
  transition: 0.2s margin-left;
  margin-left: 8px;
  min-width: 0;
  font-weight: 300;
  flex: 1;

  ${({ isIcon, selected, theme }: TitleProps) => css`
    margin-left: ${!isIcon ? 0 : 12}px;
    color: ${selected
      ? theme['tab.selected.textColor']
      : theme['tab.textColor']};
  `};
`;

export const StyledIcon = styled.div`
  height: 16px;
  min-width: 16px;
  transition: 0.2s opacity, 0.2s min-width;
  ${centerIcon()};
  ${({ isIconSet }: { isIconSet: boolean }) => css`
    min-width: ${isIconSet ? 0 : 16},
    opacity: ${isIconSet ? 0 : 1};
  `};
`;

export const StyledContent = styled.div`
  overflow: hidden;
  z-index: 2;
  align-items: center;
  display: flex;
  margin-left: 10px;
  flex: 1;
`;

export const StyledBorder = styled.div`
  position: absolute;
  width: 1px;
  height: 16px;

  right: -1px;
  top: 50%;
  transform: translateY(-50%);

  ${({ visible, theme }: { visible: boolean; theme: ITheme }) => css`
    visibility: ${visible ? 'visible' : 'hidden'};
    background-color: ${theme['toolbar.separator.color']};
  `};
`;

interface TabContainerProps {
  pinned: boolean;
  tabGroup: boolean;
}

export const TabContainer = styled.div`
  position: relative;
  border-top-left-radius: 6px;
  border-top-right-radius: 6px;
  width: 100%;
  height: calc(100% - 4px);
  overflow: hidden;
  display: flex;
  align-items: center;
  backface-visibility: hidden;
  ${({ pinned, tabGroup }: TabContainerProps) => css`
    max-width: ${pinned ? `${TAB_PINNED_WIDTH}px` : '100%'};
    border-bottom-left-radius: ${tabGroup ? 0 : 6}px;
    border-bottom-right-radius: ${tabGroup ? 0 : 6}px;
  `};
`;
