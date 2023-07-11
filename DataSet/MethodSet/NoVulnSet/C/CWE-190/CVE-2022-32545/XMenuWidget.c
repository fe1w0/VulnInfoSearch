MagickExport int XMenuWidget(Display *display,XWindows *windows,
  const char *title,const char *const *selections,char *item)
{
  Cursor
    cursor;

  int
    id,
    x,
    y;

  unsigned int
    height,
    number_selections,
    title_height,
    top_offset,
    width;

  size_t
    state;

  XEvent
    event;

  XFontStruct
    *font_info;

  XSetWindowAttributes
    window_attributes;

  XWidgetInfo
    highlight_info,
    menu_info,
    selection_info;

  XWindowChanges
    window_changes;

  /*
    Determine Menu widget attributes.
  */
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  assert(title != (char *) NULL);
  assert(selections != (const char **) NULL);
  assert(item != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",title);
  font_info=windows->widget.font_info;
  windows->widget.width=submenu_info.active == 0 ?
    WidgetTextWidth(font_info,(char *) title) : 0;
  for (id=0; selections[id] != (char *) NULL; id++)
  {
    width=WidgetTextWidth(font_info,(char *) selections[id]);
    if (width > windows->widget.width)
      windows->widget.width=width;
  }
  number_selections=(unsigned int) id;
  XGetWidgetInfo((char *) NULL,&menu_info);
  title_height=(unsigned int) (submenu_info.active == 0 ?
    (3*(font_info->descent+font_info->ascent) >> 1)+5 : 2);
  width=WidgetTextWidth(font_info,(char *) title);
  height=(unsigned int) ((3*(font_info->ascent+font_info->descent)) >> 1);
  /*
    Position Menu widget.
  */
  windows->widget.width+=QuantumMargin+(menu_info.bevel_width << 1);
  top_offset=title_height+menu_info.bevel_width-1;
  windows->widget.height=top_offset+number_selections*height+4;
  windows->widget.min_width=windows->widget.width;
  windows->widget.min_height=windows->widget.height;
  XQueryPosition(display,windows->widget.root,&x,&y);
  windows->widget.x=x-(QuantumMargin >> 1);
  if (submenu_info.active != 0)
    {
      windows->widget.x=
        windows->command.x+windows->command.width-QuantumMargin;
      toggle_info.raised=MagickTrue;
      XDrawTriangleEast(display,&windows->command,&toggle_info);
    }
  windows->widget.y=submenu_info.active == 0 ? y-(int)
    ((3*title_height) >> 2) : y;
  if (submenu_info.active != 0)
    windows->widget.y=windows->command.y+submenu_info.y;
  XConstrainWindowPosition(display,&windows->widget);
  /*
    Map Menu widget.
  */
  window_attributes.override_redirect=MagickTrue;
  (void) XChangeWindowAttributes(display,windows->widget.id,
    (size_t) CWOverrideRedirect,&window_attributes);
  window_changes.width=(int) windows->widget.width;
  window_changes.height=(int) windows->widget.height;
  window_changes.x=windows->widget.x;
  window_changes.y=windows->widget.y;
  (void) XReconfigureWMWindow(display,windows->widget.id,windows->widget.screen,
    (unsigned int) (CWWidth | CWHeight | CWX | CWY),&window_changes);
  (void) XMapRaised(display,windows->widget.id);
  windows->widget.mapped=MagickFalse;
  /*
    Respond to X events.
  */
  selection_info.height=height;
  cursor=XCreateFontCursor(display,XC_right_ptr);
  (void) XCheckDefineCursor(display,windows->image.id,cursor);
  (void) XCheckDefineCursor(display,windows->command.id,cursor);
  (void) XCheckDefineCursor(display,windows->widget.id,cursor);
  state=UpdateConfigurationState;
  do
  {
    if (state & UpdateConfigurationState)
      {
        /*
          Initialize selection information.
        */
        XGetWidgetInfo((char *) NULL,&menu_info);
        menu_info.bevel_width--;
        menu_info.width=windows->widget.width-((menu_info.bevel_width) << 1);
        menu_info.height=windows->widget.height-((menu_info.bevel_width) << 1);
        menu_info.x=(int) menu_info.bevel_width;
        menu_info.y=(int) menu_info.bevel_width;
        XGetWidgetInfo((char *) NULL,&selection_info);
        selection_info.center=MagickFalse;
        selection_info.width=menu_info.width;
        selection_info.height=height;
        selection_info.x=menu_info.x;
        highlight_info=selection_info;
        highlight_info.bevel_width--;
        highlight_info.width-=(highlight_info.bevel_width << 1);
        highlight_info.height-=(highlight_info.bevel_width << 1);
        highlight_info.x+=highlight_info.bevel_width;
        state&=(~UpdateConfigurationState);
      }
    if (state & RedrawWidgetState)
      {
        /*
          Redraw Menu widget.
        */
        if (submenu_info.active == 0)
          {
            y=(int) title_height;
            XSetBevelColor(display,&windows->widget,MagickFalse);
            (void) XDrawLine(display,windows->widget.id,
              windows->widget.widget_context,selection_info.x,y-1,
              (int) selection_info.width,y-1);
            XSetBevelColor(display,&windows->widget,MagickTrue);
            (void) XDrawLine(display,windows->widget.id,
              windows->widget.widget_context,selection_info.x,y,
              (int) selection_info.width,y);
            (void) XSetFillStyle(display,windows->widget.widget_context,
              FillSolid);
          }
        /*
          Draw menu selections.
        */
        selection_info.center=MagickTrue;
        selection_info.y=(int) menu_info.bevel_width;
        selection_info.text=(char *) title;
        if (submenu_info.active == 0)
          XDrawWidgetText(display,&windows->widget,&selection_info);
        selection_info.center=MagickFalse;
        selection_info.y=(int) top_offset;
        for (id=0; id < (int) number_selections; id++)
        {
          selection_info.text=(char *) selections[id];
          XDrawWidgetText(display,&windows->widget,&selection_info);
          highlight_info.y=selection_info.y+highlight_info.bevel_width;
          if (id == selection_info.id)
            XDrawBevel(display,&windows->widget,&highlight_info);
          selection_info.y+=(int) selection_info.height;
        }
        XDrawBevel(display,&windows->widget,&menu_info);
        state&=(~RedrawWidgetState);
      }
    if (number_selections > 2)
      {
        /*
          Redraw Menu line.
        */
        y=(int) (top_offset+selection_info.height*(number_selections-1));
        XSetBevelColor(display,&windows->widget,MagickFalse);
        (void) XDrawLine(display,windows->widget.id,
          windows->widget.widget_context,selection_info.x,y-1,
          (int) selection_info.width,y-1);
        XSetBevelColor(display,&windows->widget,MagickTrue);
        (void) XDrawLine(display,windows->widget.id,
          windows->widget.widget_context,selection_info.x,y,
          (int) selection_info.width,y);
        (void) XSetFillStyle(display,windows->widget.widget_context,FillSolid);
      }
    /*
      Wait for next event.
    */
    (void) XIfEvent(display,&event,XScreenEvent,(char *) windows);
    switch (event.type)
    {
      case ButtonPress:
      {
        if (event.xbutton.window != windows->widget.id)
          {
            /*
              exit menu.
            */
            if (event.xbutton.window == windows->command.id)
              (void) XPutBackEvent(display,&event);
            selection_info.id=(~0);
            *item='\0';
            state|=ExitState;
            break;
          }
        state&=(~InactiveWidgetState);
        if (selection_info.height == 0)
          break;
        id=(event.xbutton.y-top_offset)/(int) selection_info.height;
        selection_info.id=id;
        if ((id < 0) || (id >= (int) number_selections))
          break;
        /*
          Highlight this selection.
        */
        selection_info.y=(int) (top_offset+id*selection_info.height);
        selection_info.text=(char *) selections[id];
        XDrawWidgetText(display,&windows->widget,&selection_info);
        highlight_info.y=selection_info.y+highlight_info.bevel_width;
        XDrawBevel(display,&windows->widget,&highlight_info);
        break;
      }
      case ButtonRelease:
      {
        if (windows->widget.mapped == MagickFalse)
          break;
        if (event.xbutton.window == windows->command.id)
          if ((state & InactiveWidgetState) == 0)
            break;
        /*
          exit menu.
        */
        XSetCursorState(display,windows,MagickFalse);
        *item='\0';
        state|=ExitState;
        break;
      }
      case ConfigureNotify:
      {
        /*
          Update widget configuration.
        */
        if (event.xconfigure.window != windows->widget.id)
          break;
        if ((event.xconfigure.width == (int) windows->widget.width) &&
            (event.xconfigure.height == (int) windows->widget.height))
          break;
        windows->widget.width=(unsigned int)
          MagickMax(event.xconfigure.width,(int) windows->widget.min_width);
        windows->widget.height=(unsigned int)
          MagickMax(event.xconfigure.height,(int) windows->widget.min_height);
        state|=UpdateConfigurationState;
        break;
      }
      case EnterNotify:
      {
        if (event.xcrossing.window != windows->widget.id)
          break;
        if (event.xcrossing.state == 0)
          break;
        state&=(~InactiveWidgetState);
        if (selection_info.height == 0)
          break;
        id=((event.xcrossing.y-top_offset)/(int) selection_info.height);
        if ((selection_info.id >= 0) &&
            (selection_info.id < (int) number_selections))
          {
            /*
              Unhighlight last selection.
            */
            if (id == selection_info.id)
              break;
            selection_info.y=(int)
              (top_offset+selection_info.id*selection_info.height);
            selection_info.text=(char *) selections[selection_info.id];
            XDrawWidgetText(display,&windows->widget,&selection_info);
          }
        if ((id < 0) || (id >= (int) number_selections))
          break;
        /*
          Highlight this selection.
        */
        selection_info.id=id;
        selection_info.y=(int)
          (top_offset+selection_info.id*selection_info.height);
        selection_info.text=(char *) selections[selection_info.id];
        XDrawWidgetText(display,&windows->widget,&selection_info);
        highlight_info.y=selection_info.y+highlight_info.bevel_width;
        XDrawBevel(display,&windows->widget,&highlight_info);
        break;
      }
      case Expose:
      {
        if (event.xexpose.window != windows->widget.id)
          break;
        if (event.xexpose.count != 0)
          break;
        state|=RedrawWidgetState;
        break;
      }
      case LeaveNotify:
      {
        if (event.xcrossing.window != windows->widget.id)
          break;
        state|=InactiveWidgetState;
        id=selection_info.id;
        if ((id < 0) || (id >= (int) number_selections))
          break;
        /*
          Unhighlight last selection.
        */
        selection_info.y=(int) (top_offset+id*selection_info.height);
        selection_info.id=(~0);
        selection_info.text=(char *) selections[id];
        XDrawWidgetText(display,&windows->widget,&selection_info);
        break;
      }
      case MotionNotify:
      {
        /*
          Discard pending button motion events.
        */
        while (XCheckMaskEvent(display,ButtonMotionMask,&event)) ;
        if (submenu_info.active != 0)
          if (event.xmotion.window == windows->command.id)
            {
              if ((state & InactiveWidgetState) == 0)
                {
                  if (MatteIsActive(submenu_info,event.xmotion) == MagickFalse)
                    {
                      selection_info.id=(~0);
                        *item='\0';
                      state|=ExitState;
                      break;
                    }
                }
              else
                if (WindowIsActive(windows->command,event.xmotion))
                  {
                    selection_info.id=(~0);
                    *item='\0';
                    state|=ExitState;
                    break;
                  }
            }
        if (event.xmotion.window != windows->widget.id)
          break;
        if (state & InactiveWidgetState)
          break;
        if (selection_info.height == 0)
          break;
        id=(event.xmotion.y-top_offset)/(int) selection_info.height;
        if ((selection_info.id >= 0) &&
            (selection_info.id < (int) number_selections))
          {
            /*
              Unhighlight last selection.
            */
            if (id == selection_info.id)
              break;
            selection_info.y=(int)
              (top_offset+selection_info.id*selection_info.height);
            selection_info.text=(char *) selections[selection_info.id];
            XDrawWidgetText(display,&windows->widget,&selection_info);
          }
        selection_info.id=id;
        if ((id < 0) || (id >= (int) number_selections))
          break;
        /*
          Highlight this selection.
        */
        selection_info.y=(int) (top_offset+id*selection_info.height);
        selection_info.text=(char *) selections[id];
        XDrawWidgetText(display,&windows->widget,&selection_info);
        highlight_info.y=selection_info.y+highlight_info.bevel_width;
        XDrawBevel(display,&windows->widget,&highlight_info);
        break;
      }
      default:
        break;
    }
  } while ((state & ExitState) == 0);
  (void) XFreeCursor(display,cursor);
  window_attributes.override_redirect=MagickFalse;
  (void) XChangeWindowAttributes(display,windows->widget.id,
    (size_t) CWOverrideRedirect,&window_attributes);
  (void) XWithdrawWindow(display,windows->widget.id,windows->widget.screen);
  XCheckRefreshWindows(display,windows);
  if (submenu_info.active != 0)
    {
      submenu_info.active=MagickFalse;
      toggle_info.raised=MagickFalse;
      XDrawTriangleEast(display,&windows->command,&toggle_info);
    }
  if ((selection_info.id < 0) || (selection_info.id >= (int) number_selections))
    return(~0);
  (void) CopyMagickString(item,selections[selection_info.id],MaxTextExtent);
  return(selection_info.id);
}