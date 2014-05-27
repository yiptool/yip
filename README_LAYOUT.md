
# User interface layout file format

User interface layout file is an XML file.

Root element should be named `layout` and should contain a mandatory attribute `size` which
contains a pair of comma-separated floating-point numbers representing width and height for
which this UI layout was originally designed.

Each non-root element in this file represents a UI widget or a widget group. Widget group may
have children widgets or groups.

## Widgets

The following widgets are available:

  * `group`: a group of widgets.
  * `label`: a block of text.
  * `image`: a static image.
  * `button`: a clickable element with a background, optional icon and/or optional title.

The following attributes are available for all widgets and groups:

  * `id`: unique ID of the widget - **this attribute is mandatory**.
  * `bgcolor`: background color of the widget.
  * `pos`: X and Y coordinates of the top left corner of the widget, relative to it's parent group.
  * `size`: width and height of the widget.
  * `scale`: scale mode.

### Unique ID

ID of the widget is used to uniquely identify the widget. This should conform to the programming language's
rules of identifiers (usually it should start with a letter or an underscore and may contain only letters,
digits and underscores). This should also be unique - that is, no other widget in the layout file should
have the same identifier.

### Position and size

Position and size should be specified as a pair of comma-separated floating-point numbers. The default
value for both is `0, 0`.

### Colors

The following values may be used for colors:

  * `clear`
  * `black`
  * `darkgray`
  * `lightgray`
  * `white`
  * `gray`
  * `red`
  * `green`
  * `blue`
  * `cyan`
  * `yellow`
  * `magenta`
  * `orange`
  * `purple`
  * `brown`
  * `#RGB` (*R*, *G* and *B* are hexadecimal digits)
  * `#RGBA` (*R*, *G*, *B* and *A* are hexadecimal digits)
  * `#RRGGBB` (*R*, *G* and *B* are hexadecimal digits)
  * `#RRGGBBAA` (*R*, *G*, *B* and *A* are hexadecimal digits)

### Scale mode

For each coordinate you may specify scale mode. This is achieved using the following attributes:

  * `xscale`: sets scale mode for the X coordinate of the top left corner.
  * `yscale`: sets scale mode for the Y coordinate of the top left corner.
  * `wscale`: sets scale mode for the width of the widget.
  * `hscale`: sets scale mode for the height of the widget.
  * `xwscale`: sets the same scale mode for both X coordinate of the top left corner
      and for width of the widget.
  * `yhscale`: sets the same scale mode for both Y coordinate of the top left corner
      and for height of the widget.
  * `xyscale`: sets the same scale mode for both X and Y coordinates of the top left corner of the widget.
  * `whscale`: sets the same scale mode for both width and height of the widget.
  * `scale`: sets the same scale mode for all coordinates and dimensions of the widget.

The following values are available for the scale mode:

  * `default`: use horizontal scale ratio for X coordinate and width; use vertical scale ratio
      for Y coordinate and height.
  * `min`: use minimum of two scale ratios.
  * `max`: use maximum of two scale ratios.
  * `horz`: use horizontal scale ratio.
  * `vert`: use vertical scale ratio.
  * `avg`: use an average of horizontal and vertical scale ratio.
