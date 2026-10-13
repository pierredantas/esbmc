<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- Under --ld-closed-world the shared preset p is written by this program only, so it is a constant. -->
      <interface>
        <inputVars><variable name="x"><type><BOOL/></type></variable></inputVars>
        <externalVars><variable name="p"><type><INT/></type><initialValue><simpleValue value="3"/></initialValue></variable></externalVars>
        <localVars>
          <variable name="q"><type><BOOL/></type></variable>
          <variable name="et"><type><INT/></type></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><block typeName="TON" instanceName="t1"><variable formalParameter="IN">x</variable><variable formalParameter="PT">p</variable><variable formalParameter="Q">q</variable><variable formalParameter="ET">et</variable></block></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
