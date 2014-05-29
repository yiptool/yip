
# Translation file format

Translation file is an XML file.

Root element should be named `translations`.

Each non-root element should be named `string`. It should not have child elements, but should
have two mandatory attributes: `f` and `t`. The `f` attribute should contain an untranslated
string and the `t` attribute should contain a translation for the string. If the `t` attribute
contains a special value `TBT`, the string will be considered untranslated.
