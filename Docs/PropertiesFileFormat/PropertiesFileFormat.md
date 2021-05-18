# PropertiesFileFormat

The file contains properties of the following format:
	
	<IDENTIFIER> = <VALUE> ; 

`IDENTIFIER` must satisfy this regular expression:

	IDENTIFIER = ^[_a-zA-Z][_a-zA-Z0-9]+$

Cand start with a letter or underscore character `'_'`, followed by any number of letters, digits and underscore character `'_'`.

Itentifier is case sensitive.

`VALUE` can be a simple value or an array that contains simple values.

# Simple values are: Int, Float, String

## `Int` integer number

	INTEGER LITERAL = ^[-+]?[0-9]+$

Optional `'-'`, `'+'` characters in front to specify sign.

Digit 0 in front is ignored. `012 = 12`.

Example:

	name_of_int_property = 1523;

## `Float` floating point number

	FLOAT LITERAL = ^[-+]?[0-9]+[.][0-9]+$
	
Optional `'-'`, `'+'` characters in front to specify sign.

Digit `0` in front is ignored. `012.3 = 12.3` But `0.54 = 0.54`

Decimal separator is character '.' decimal point.

Example: 

	name_of_float_property = -43.123;

## `String` string of text

	STRING LITERAL    = ^"[^"]*"$

It must start with character `'"'` and finish with it.

In the middle of the string you cannot use character `'"'`.

Example:

	name_of_string_property = "qwerty";

## `Array` is a list of simple values.

It starts with `'['` and ends with `']'` characters.

In the middle are simple values separated with `','`.

In an array you can mix simple values of different types.

Example:

	name_of_array_property = [+12, 5.33, "qwerty"];

# Comments

There are 2 types of comments: 

## Single line comments

	LINE_COMMENT = ^[\/][\/][^\n\r]*[\r\n\0]$

Start with character sequence `"//"` and followed by any character till the end of the line.

## Multiline comments 

	MULTILINE_COMMENT = ^[\/][*][^*\/]*[*][\/]$

Start with character sequence `"/*"` followed by any character and ends with character sequence `"*/"`.

# Whitespaces

	WHITESPACE = ^[ \r\n\t]$

Between tokens and comments can be any number of whitespace characters.
A whitespace character is `'\t'`, `' '`, `'\r'`, `'\n'`.

# Syntax diagram

	SyntaxDiagramPropertiesFileFormat.drawio
	SyntaxDiagramPropertiesFileFormat.png

# All regular expressions to parse tokens:

	IDENTIFIER        = ^[_a-zA-Z][_a-zA-Z0-9]+$
	INTEGER LITERAL   = ^[-+]?[0-9]+$
	FLOAT LITERAL     = ^[-+]?[0-9]+[.][0-9]+$
	STRING LITERAL    = ^"[^"]*"$

	WHITESPACE        = ^[ \r\n\t]$
	LINE_COMMENT      = ^[\/][\/][^\n\r]*[\r\n\0]$
	MULTILINE_COMMENT = ^[\/][*][^*\/]*[*][\/]$

