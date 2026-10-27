# Copyright András Vukics 2026. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE.txt)
# Doxygen input filter for README.md (sed -E): Doxygen 1.9 does not understand GitHub's $…$ and $$…$$ math,
# so convert them to \f$…\f$ and \f[…\f]. Display math must sit on a line of its own.
s/^\$\$(.*)\$\$$/\\f[\1\\f]/
s/\$([^$]+)\$/\\f$\1\\f$/g
