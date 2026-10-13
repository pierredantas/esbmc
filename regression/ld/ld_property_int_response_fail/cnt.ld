<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- Cnt is an INT; a property atom must be BOOL. -->
      <interface>
        <inputVars><variable name="a"><type><BOOL/></type></variable></inputVars>
        <localVars>
          <variable name="y"><type><BOOL/></type></variable>
          <variable name="Cnt"><type><INT/></type><initialValue><simpleValue value="2"/></initialValue></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><contact variable="a"/><coil variable="y"/></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
