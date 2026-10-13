<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- A MOVE into the counter's CV would change the converter's count, while
           MATIEC's instance keeps its own CV. -->
      <interface>
        <inputVars><variable name="cu"><type><BOOL/></type></variable></inputVars>
        <localVars>
          <variable name="q"><type><BOOL/></type></variable>
          <variable name="cv"><type><INT/></type></variable>
          <variable name="pv"><type><INT/></type><initialValue><simpleValue value="3"/></initialValue></variable>
          <variable name="zero"><type><INT/></type></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><block typeName="CTU" instanceName="c1"><variable formalParameter="CU">cu</variable><variable formalParameter="PV">pv</variable><variable formalParameter="Q">q</variable><variable formalParameter="CV">cv</variable></block></rung>
          <rung localId="2"><block typeName="MOVE" instanceName="m"><variable formalParameter="IN1">zero</variable><variable formalParameter="OUT">cv</variable></block></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
