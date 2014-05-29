
# User interface layout file format

User interface layout file is an XML file.

Root element should be named `layout` and should contain some mandatory attributes:

  * `size` is a size of the layout; it may have one of the following formats: `xx,yy` or `xx,yy/xx,yy`.
  Here `xx` and `yy` are floating-point numbers representing width and height respectively. This numbers
  represent an initialial size of the layout, that is - the original size of the mockup. In the latter form
  (the one with the slash), first pair represents dimensions of the portrait layout and the second pair
  represents dimensions of the landscape layout.

  * `portrait` is a boolean value (either `yes` or `no`); set it to `yes` if your layout supports portrait
  orientations.

  * `landscape` is a boolean value (either `yes` or `no`); set it to `yes` if your layout supports landscape
  orientations.

Each non-root element in the UI file represents a UI widget or a widget group. Widget group may
have children widgets or groups.

## Widgets

The following widgets are available:

  * `group`: a group of widgets.
  * `label`: a block of text.
  * `image`: a static image.
  * `button`: a clickable element with a background, optional icon and/or optional title.

The following attributes are available for all widgets and groups:

  * `id`: unique ID of the widget.
  * `bgcolor`: background color of the widget.
  * `pos`: Comma-separated X and Y coordinates of the top left corner of the widget, relative to it's
  parent group. You may specify two pairs, separated with a slash (`/`) - the first one will represent
  position for the portrait orientation and the second one will represent position for the landscape
  orientation.
  * `size`: Comma-separated width and height of the widget. You may specify two pairs, separated with
  a slash (`/`) - the first one will represent size for the portrait orientation and the second one
  will represent size for the landscape orientation.
  * `scale`: scale mode. You may specify two scale modes, separated with a slash (`/`) - one for the
  portrait orientation and one for the landscape orientation.
  * `align`: alignment of widget after applying scale (this is only meaningful when scale mode is not
  `default`). You may specify two alignments, separated with a slash (`/`) - one for the
  portrait orientation and one for the landscape orientation.

### Unique ID

ID of the widget is used to uniquely identify the widget. This should conform to the programming language's
rules of identifiers (usually it should start with a letter or an underscore and may contain only letters,
digits and underscores). This should also be unique - that is, no other widget in the layout file should
have the same identifier.

### Position and size

Position and size should be specified as a pair of comma-separated floating-point numbers. You may specify
two pairs, separated with a slash (`/`) - the first one will represent position or size for the portrait
orientation and the second one - for the landscape orientation.

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

You may specify two scale modes, separated with a slash (`/`) - one for the portrait orientation and one
for the landscape orientation.

### Alignment

When scaling is applied to the widget and scale mode is not `default`, widget size will differ from
the size of the area allocated for it. In this case, position of the widget inside of this area can
be specified with the `align` attribute.

Alignment is a comma-separated list of one or more of the following values:

  * `hcenter`: center widget horizontally.
  * `left`: bind left side of the widget to the left side of the allocated area.
  * `right`: bind right side of the widget to the right side of the allocated area.
  * `vcenter`: center widget vertically.
  * `top`: bind top side of the widget to the top side of the allocated area.
  * `bottom`: bind bottom side of the widget to the bottom side of the allocated area.

Also the following shortcuts are available:

  * `center` is the same as `vcenter,hcenter`
  * `tl` is the same as `top,left`
  * `bl` is the same as `bottom,left`
  * `tr` is the same as `top,right`
  * `br` is the same as `bottom,right`

You may specify two alignments, separated with a slash (`/`) - one for the portrait orientation and one for
the landscape orientation.

## Button

For a button you may additionaly specify the following attributes:

  * `title`: text to display on the button.
  * `image`: icon to display on the button.
