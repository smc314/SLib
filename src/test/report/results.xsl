<?xml version="1.0" encoding="UTF-8"?>

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:template match="/Catch">
        <html>
            <head>
                <title>SLib Test Results</title>
                <script>
                    function toggle(Id)
                    {
                        var element = document.getElementById(Id);
                        if(element == null)
                            return;

                        if(element.style.display == "block")
                            element.style.display = "none";
                        else
                            element.style.display = "block";
                    }
                </script>
            </head>

            <body>
                <div onclick="toggle('#{generate-id(.)}_groups'); toggle('#{generate-id(.)}_groupCnt');">
                    <h1>SLib Test Results</h1>

                    <xsl:apply-templates select="OverallResults" />
                    <xsl:if test="Group">
                        <p id="#{generate-id(.)}_groupCnt" style="display:block;"><xsl:value-of select="count(Group)" /> Groups</p>
                    </xsl:if>
                </div>
                <div id="#{generate-id(.)}_groups" style="display:none;">
                    <xsl:apply-templates select="Group" />
                    <xsl:if test="Group">
                        <p onclick="toggle('#{generate-id(.)}_groups'); toggle('#{generate-id(.)}_groupCnt');">Collapse</p>
                    </xsl:if>
                </div>
            </body>
        </html>
    </xsl:template>

    <xsl:template match="OverallResults">
        <table>
            <tr>
                <th>Total</th>
                <th>Successes</th>
                <th>Failures</th>
                <th>Expected</th>
            </tr>
            <tr>
                <td><xsl:value-of select="@successes + @failures" /></td>
                <td><xsl:value-of select="@successes" /></td>
                <td><xsl:value-of select="@failures" /></td>
                <td><xsl:value-of select="@expectedFailures" /></td>
            </tr>
        </table>
    </xsl:template>

    <xsl:template match="Group">
        <div id="#{generate-id(.)}" style="margin-left: 2em">
            <div onclick="toggle('#{generate-id(.)}_cases'); toggle('#{generate-id(.)}_caseCnt');">
                <h2>Group - <xsl:value-of select="@name" /></h2>

                <xsl:apply-templates select="OverallResults" />
                <xsl:if test="TestCase">
                    <p id="#{generate-id(.)}_caseCnt" style="display:block;"><xsl:value-of select="count(TestCase)" /> Test Cases</p>
                </xsl:if>
            </div>
            <div id="#{generate-id(.)}_cases" style="display:none;">
                <xsl:apply-templates select="TestCase" />
                <xsl:if test="TestCase">
                    <p onclick="toggle('#{generate-id(.)}_cases'); toggle('#{generate-id(.)}_caseCnt');">Collapse</p>
                </xsl:if>
            </div>
            <hr />
        </div>

    </xsl:template>

    <xsl:template match="TestCase">
        <div id="#{generate-id(.)}" style="margin-left: 2em">
            <div onclick="toggle('#{generate-id(.)}_sections'); toggle('#{generate-id(.)}_sectionCnt');">
                <h3>Test Case - <xsl:value-of select="@name" /></h3>
                <p>
                    <xsl:value-of select="@filename" />:<xsl:value-of select="@line" />
                    - Tags: <xsl:value-of select="@tags" />
                </p>
                <xsl:choose>
                    <xsl:when test="OverallResult/@success = 'true'">
                        Success
                    </xsl:when>
                    <xsl:otherwise>
                        Failure
                    </xsl:otherwise>
                </xsl:choose><br />
                <xsl:if test="Expression">
                    At <xsl:value-of select="Expression/@filename" />:<xsl:value-of select="Expression/@line" />
                    <br />FAILED:<br />
                    <code style="margin-left: 4ch">
                        <xsl:value-of select="Expression/@type" />(
                        <xsl:value-of select="Expression/Original" />)</code><br />
                    <xsl:choose>
                        <xsl:when test="Expression/@type = 'REQUIRE_THROWS'">
                            because no exception was thrown when one was expected.
                        </xsl:when>
                        <xsl:when test="Expression/@type = 'REQUIRE_NOTHROW'">
                            because an exception was thrown when none was expected.
                        </xsl:when>
                        <xsl:otherwise>
                            with expansion:<br />
                            <code style="margin-left: 4ch"><xsl:value-of select="Expression/Expanded" /></code>
                            <xsl:if test="Info">
                                <br />with message:<br />
                                <code style="margin-left: 4ch"><xsl:value-of select="Info" /></code>
                            </xsl:if>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:if>
                <xsl:if test="Exception">
                    At <xsl:value-of select="Exception/@filename" />:<xsl:value-of select="Exception/@line" />
                    <br />FAILED:<br />
                    due to an unexpected exception with message:<br />
                    <code style="margin-left: 4ch">
                        <xsl:value-of select="Exception" />
                    </code>
                </xsl:if>
                <xsl:if test="Section">
                    <p id="#{generate-id(.)}_sectionCnt" style="display:block;"><xsl:value-of select="count(Section)" /> Sections</p>
                </xsl:if>
                <xsl:if test="FatalErrorCondition">
                    At <xsl:value-of select="FatalErrorCondition/@filename" />:<xsl:value-of select="FatalErrorCondition/@line" />
                    <br />FAILED:<br />
                    due to a fatal error condition:<br />
                    <code style="margin-left: 4ch">
                        <xsl:value-of select="FatalErrorCondition" />
                    </code>
                </xsl:if>
            </div>

            <div id="#{generate-id(.)}_sections" style="display:none;">
                <xsl:apply-templates select="Section" />
                <xsl:if test="Section">
                    <p onclick="toggle('#{generate-id(.)}_sections'); toggle('#{generate-id(.)}_sectionCnt');">Collapse</p>
                </xsl:if>
            </div>
            <hr />
        </div>
    </xsl:template>

    <xsl:template match="Section">
        <div id="#{generate-id(.)}" style="margin-left: 2em">
            <div onclick="toggle('#{generate-id(.)}_subsections'); toggle('#{generate-id(.)}_subsectionCnt');">
                <h3>Section - <xsl:value-of select="@name" /></h3>

                <p>
                    <xsl:value-of select="@filename" />:<xsl:value-of select="@line" />
                </p>

                <xsl:apply-templates select="OverallResults" />
                <xsl:if test="Expression">
                    At <xsl:value-of select="Expression/@filename" />:<xsl:value-of select="Expression/@line" />
                    <br />FAILED:<br />
                    <code style="margin-left: 4ch">
                        <xsl:value-of select="Expression/@type" />(
                        <xsl:value-of select="Expression/Original" />)</code><br />
                    <xsl:choose>
                        <xsl:when test="Expression/@type = 'REQUIRE_THROWS'">
                            because no exception was thrown when one was expected.
                        </xsl:when>
                        <xsl:when test="Expression/@type = 'REQUIRE_NOTHROW'">
                            because an exception was thrown when none was expected.
                        </xsl:when>
                        <xsl:otherwise>
                            with expansion:<br />
                            <code style="margin-left: 4ch"><xsl:value-of select="Expression/Expanded" /></code>
                            <xsl:if test="Info">
                                <br />with message:<br />
                                <code style="margin-left: 4ch"><xsl:value-of select="Info" /></code>
                            </xsl:if>
                        </xsl:otherwise>
                    </xsl:choose>
                </xsl:if>
                <xsl:if test="Exception">
                    At <xsl:value-of select="Exception/@filename" />:<xsl:value-of select="Exception/@line" />
                    <br />FAILED:<br />
                    due to an unexpected exception with message:<br />
                    <code style="margin-left: 4ch">
                        <xsl:value-of select="Exception" />
                    </code>
                </xsl:if>
                <xsl:if test="FatalErrorCondition">
                    At <xsl:value-of select="FatalErrorCondition/@filename" />:<xsl:value-of select="FatalErrorCondition/@line" />
                    <br />FAILED:<br />
                    due to a fatal error condition:<br />
                    <code style="margin-left: 4ch">
                        <xsl:value-of select="FatalErrorCondition" />
                    </code>
                </xsl:if>
                <xsl:if test="Section">
                    <p id="#{generate-id(.)}_subsectionCnt" style="display:block;"><xsl:value-of select="count(Section)" /> Subsections</p>
                </xsl:if>
            </div>
            <div id="#{generate-id(.)}_subsections" style="display:none;">
                <xsl:apply-templates select="Section" />
                <xsl:if test="Section">
                    <p onclick="toggle('#{generate-id(.)}_subsections'); toggle('#{generate-id(.)}_subsectionCnt');">Collapse</p>
                </xsl:if>
            </div>
            <hr />
        </div>
    </xsl:template>

</xsl:transform>
