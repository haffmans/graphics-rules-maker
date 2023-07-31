# Read input file
cmake_minimum_required(VERSION 3.24)

file(STRINGS "${PCI_INPUT}" PCI_VENDORS REGEX "^[0-9a-f]")

foreach(PCI_VENDOR_LINE IN LISTS PCI_VENDORS)
    string(SUBSTRING "${PCI_VENDOR_LINE}" 0 4 PCI_VENDOR_ID)
    string(SUBSTRING "${PCI_VENDOR_LINE}" 4 -1 PCI_VENDOR_NAME_UNTRIMMED)
    string(STRIP "${PCI_VENDOR_NAME_UNTRIMMED}" PCI_VENDOR_NAME_DIRTY)

    # Some vendor names have "???" in there. To avoid them becoming tri-graphs, escape all question marks
    string(REPLACE "?" "\\?" PCI_VENDOR_NAME "${PCI_VENDOR_NAME_DIRTY}")

    string(CONFIGURE "        {0x@PCI_VENDOR_ID@, \"@PCI_VENDOR_NAME@\"}" PCI_VENDOR_ITEM @ONLY ESCAPE_QUOTES)
    list(APPEND PCI_VENDOR_ITEMS "${PCI_VENDOR_ITEM}")
endforeach()

list(JOIN PCI_VENDOR_ITEMS ",\n" PCI_VENDOR_LIST)

configure_file("${PCI_TEMPLATE}" "${PCI_OUTPUT}" @ONLY)
