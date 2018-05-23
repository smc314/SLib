<?xml version="1.0" encoding="UTF-8"?>

<xsl:transform version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

    <xsl:template match="/Catch">
        <!DOCTYPE HTML>
        <html>
            <head>
                <title>SLib Test Results</title>
            </head>

            <body>
                <h1>SLib Test Results</h1>

                <xsl:apply-templates select="OverallResults" />
                <xsl:apply-templates select="Group" />
            </body>
        </html>
    </xsl:template>

    <xsl:template match="OverallResults">
        <table>
            <tr>
                <th>Successes</th>
                <th>Failures</th>
                <th>Expected</th>
            </tr>
            <tr>
                <td><xsl:value-of select="@successes" /></td>
                <td><xsl:value-of select="@failures" /></td>
                <td><xsl:value-of select="@expectedFailures" /></td>
            </tr>
        </table>
    </xsl:template>

    <xsl:template match="Group">
        <div style="margin-left: 2em">
            <h2>Group - <xsl:value-of select="@name" /></h2>

            <xsl:apply-templates select="OverallResults" />
            <xsl:apply-templates select="TestCase" />
            <hr />
        </div>

    </xsl:template>

    <xsl:template match="TestCase">
        <div style="margin-left: 2em">
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
            </xsl:choose>

            <xsl:apply-templates select="Section" />
            <hr />
        </div>
    </xsl:template>

    <xsl:template match="Section">
        <div style="margin-left: 2em">
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
                with expansion:<br />
                <code style="margin-left: 4ch"><xsl:value-of select="Expression/Expanded" /></code>
                <xsl:if test="Info">
                    <br />with message:<br />
                    <code style="margin-left: 4ch"><xsl:value-of select="Info" /></code>
                </xsl:if>
            </xsl:if>
            <xsl:apply-templates select="Section" />
            <hr />
        </div>
    </xsl:template>

</xsl:transform>
