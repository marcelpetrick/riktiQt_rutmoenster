# riktiQt_rutmoenster
riktiQt rutmönster - small helper to convert the Qt gridlayout ui-files (after changes) into a unified state

############
# 20180530 #
############
* original blog-entry describing the project: http://marcelpetrick.bplaced.net/wp_solutionsnotcode/?page_id=121 to the repository<br>
* builds against MSVC2013 and Qt 5.5
* current version 0.02
* contact: mail@marcelpetrick.it

#################
# original text #
#################
riktiQt rutmönster
The forms which use a gridlayout in Qt share a certain weakness. Although I think the QtDesigner should handle this, this is a source of annoyance: if the items for the gridlayout are not inserted in the order they should appear (sorted from:top-left to bottom-right, rows before colums) then tabbing (runtime: user wants to use TAB to switch between ui-elements) does not work. Also the difference between the original state and the newly saved state looks quite ugly.
So I created riktiQt rutmönster (swedish: ‘correct grid’).

It’s a command line-tool to parse and sort the ui-form part according to the described order. It is a simplified LR(1)_parser and therefore works even with nested layouts/items. Even the attribute ‘colspan’ is considered.
Currently it does not keep the old formatting of the XML, therefore loading & re-saving in QtDesigner is needed.
I also skipped implementing some error-checks like “does amount of lines correspond?” “is every item from input contained in output?”.

It was tested on some bigger forms with a three level-deep nesting. Later used in production cycles by at least a colleauge and me.

Sources:
* first release: 0.01

How to compile:
* open the pro-file and create a buildable project with your personal settings ..
* tested with GCC 4.7; Qt 4.8 on Linux

How to use:
* $ ./CLexer input.ui 2> error.txt > output.ui
* first parameter is the input-filename; hints & errors are redirected to stderr; the result is printed on stdout

Forecast for the future:
* prettified source with the proper name
* releasing the original Git-repository
* error-checks
* keeping the original alignment of the input (intendation, …)
* keeping the order for the trivial properties (currently they are added at the end of their “item”)
* GUI which simplifies selecting input and the output
* directly writing into an output-file

If you have more ideas – tell me: mail@marcelpetrick.it
