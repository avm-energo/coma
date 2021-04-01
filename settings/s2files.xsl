<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" 
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:template match="/">
 <html>
 <body>
	<TABLE BORDER="1" CELLPADDING="5">
		<THEAD>
            <TH>ИД</TH>
            <TH>Тип</TH>
						<TH>Описание</TH>
			<TH>Виджет</TH>
			<TH>Группа</TH>

        </THEAD>
        <xsl:for-each select="s2files/record">
            <TR ALIGN="CENTER">
               <TD>
                  <xsl:value-of select="id"/>
               </TD>
			   <TD>
                  <xsl:value-of select="type"/>
               </TD>
			   			   			   <TD>
                  <xsl:value-of select="widget/string"/>
               </TD>
			   <TD>
                  <xsl:value-of select="widget/type"/>
               </TD>
			   			   <TD>
                  <xsl:value-of select="widget/group"/>
               </TD>

            </TR>
         </xsl:for-each>
      </TABLE>
	  </body>
</html>
</xsl:template>
</xsl:stylesheet>