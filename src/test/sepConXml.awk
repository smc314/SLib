#! /usr/bin/awk -f

BEGIN { printf "" > "test/report/results.xml" }

/<[\/\?]?[[:alpha:]]+( [[:alpha:]]+=".*")*[\/\?]?>/ { print $0 >> "test/report/results.xml" }
/<\?xml .*\?>/ { print "<?xml-stylesheet type=\"text/xsl\" href=\"results.xsl\"?>" >> "test/report/results.xml" }
/<Info>/ {
    while($0 !~ /.*<\/Info>.*/)
    {
        getline
        print $0 >> "test/report/results.xml"
    }
}

/<Original>/ {
    while($0 !~ /.*<\/Original>.*/)
    {
        getline
        print $0 >> "test/report/results.xml"
    }
}

/<Expanded>/ {
    while($0 !~ /.*<\/Expanded>.*/)
    {
        getline
        print $0 >> "test/report/results.xml"
    }
}

$0 !~ /.*<[\/\?]?[[:alpha:]]+( [[:alpha:]]+=".*")*[\/\?]?>.*/ { print $0 }
