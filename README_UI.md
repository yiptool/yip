
# User interface layout file format

User interface layout file is an XML file.

Root element should be named `layout` and should contain some mandatory attributes:

  * `size` is a size of the layout; it may have one of the following formats: `xx,yy` or `xx,yy/xx,yy`.
  Here `xx` and `yy` are floating-point numbers representing width and height respectively. This numbers
  represent an initialial size of the layout, that is - the original size of the mockup. In the latter form
  (the one with the slash), first pair represents dimensions of the portrait layout and the second pair
  represents dimensions of the landscape layout.

  * `portrait` is a boolean value (either `yes` or `no`); set it to `yes` if your layout supports portrait
  orientations. You can set two boolean values separated with a slash (`/`) for this attribute. The first
  one will be used for phones and the second one will be used for tablets.

  * `landscape` is a boolean value (either `yes` or `no`); set it to `yes` if your layout supports landscape
  orientations. You can set two boolean values separated with a slash (`/`) for this attribute. The first
  one will be used for phones and the second one will be used for tablets.

Each non-root element (except `string`) in the UI file represents a UI widget or a widget group. Widget group may
have children widgets or groups.

You can also specify translatable string constants using the `string` element. It should have two mandatory
attributes: `id` and `text`. Attribute `id` should contain a name for a property or variable to be generated.
It will be of string type and will contain value of `text` translated to the current user's language.

## Widgets

The following widgets are available:

  * `group`: a group of widgets.
  * `scrollview`: a scrollable group of widgets.
  * `label`: a block of text.
  * `image`: a static image.
  * `switch`: a clickable switch with two possible values (usually "ON" and "OFF").
  * `button`: a clickable element with a background, optional icon and/or optional title.
  * `textfield`: a text input field.
  * `spinner`: a spinner widget.
  * `tableview`: a view that shows items in a vertically scrolling list.
  * `webview`: a browser view.

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
  * `#ARGB` (*R*, *G*, *B* and *A* are hexadecimal digits)
  * `#RRGGBB` (*R*, *G* and *B* are hexadecimal digits)
  * `#AARRGGBB` (*R*, *G*, *B* and *A* are hexadecimal digits)

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
  * `none`: always use scale ratio of 1.

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

### Font

Some widgets allow to specify a font to use to display a text. The font string consists of a font name,
followed by a comma (`,`) and by a font size in points. You can optionally specify two font sizes separated
with a slash (`/`) - one for the portrait orientation and one for the landscape orientation.

Here are some examples of font specifications:

  * `Arial,16`
  * `OpenSans,16/20`

## Switch

For a switch you may specify custom images for the "on" state, "off" state and for the moving "knob".
You should either specify all three images or do not specify any of them.

To specify custom images for the switch, use the following attributes:

  * `knobImage`: name of the resource containing image for the moving "knob".
  * `onImage`: name of the resource containing image for the "on" state.
  * `offImage`: name of the resource containing image for the "off" state.

## Button

For a button you may additionaly specify the following attributes:

  * `image`: icon to display on the button.
  * `bgimage`: background image for the button.
  * `text`: text to display on the button.
  * `textColor`: color of the button text.
  * `font`: font for the button text.
  * `fontScale`: scale mode for the button text font. You may specify two scale modes, separated with a
  slash (`/`) - one for the portrait orientation and one for the landscape orientation.

## Label

For a label you may additionaly specify the following attributes:

  * `text`: text to display on the label.
  * `textColor`: color of the label text.
  * `font`: font for the label text.
  * `fontScale`: scale mode for the label text font. You may specify two scale modes, separated with a
  slash (`/`) - one for the portrait orientation and one for the landscape orientation.
  * `textAlign`: alignment of the text, one of `left`, `right` or `center`.

## Text Field

For a text field you may additionaly specify the following attributes:

  * `text`: placeholder text to display in the field when it is empty.
  * `textColor`: color of the text.
  * `font`: font for the text.
  * `fontScale`: scale mode for the text font. You may specify two scale modes, separated with a
  slash (`/`) - one for the portrait orientation and one for the landscape orientation.
  * `textAlign`: alignment of the text, one of `left`, `right` or `center`.

## Spinner

For a spinner you may additionally specify the following attributes:

  * `style`: style of a spinner, could be one of `small_dark`, `small_light` or `large_light`.

## Image

For an image you may additionaly specify the following attributes:

  * `image`: path to the image resource.

## Table view

Table view may contain cell views as subelements. They should be declared using the `cell` element:

    <tableview pos="0,0" size="100,100">
      <cell className="TableCell" height="50">
        <label pos="0,0" size="100,50" />
      </cell>
    </tableview>

A separate class is created for each cell. You have to always specify the `className` attribute with
a valid class name. It will also be used as a *reusable cell identifier* on the iOS platform.

Also, you are obligated to specify the `height` attribute containing a height of a cell. You may also
specify two heights, separated with a slash (`/`) - one for the portrait orientation and one for the
landscape orientation.

The following optional attributes are available for the `cell` element:

  * `ios:parentClass`: name of a parent class on the iOS platform. Default is `UITableViewCell`.
  * `android:parentClass`: name of a parent class on the Android platform. Default is `android.view.ViewGroup`.
