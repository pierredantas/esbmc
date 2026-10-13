<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- A MOVE writes the preset each scan, so it can change mid-interval. -->
      <interface>
        <inputVars><variable name="x"><type><BOOL/></type></variable></inputVars>
        <localVars>
          <variable name="q"><type><BOOL/></type></variable>
          <variable name="et"><type><INT/></type></variable>
          <variable name="p"><type><INT/></type><initialValue><simpleValue value="3"/></initialValue></variable>
          <variable name="five"><type><INT/></type><initialValue><simpleValue value="5"/></initialValue></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><block typeName="MOVE" instanceName="m"><variable formalParameter="IN1">five</variable><variable formalParameter="OUT">p</variable></block></rung>
          <rung localId="2"><block typeName="TON" instanceName="t1"><variable formalParameter="IN">x</variable><variable formalParameter="PT">p</variable><variable formalParameter="Q">q</variable><variable formalParameter="ET">et</variable></block></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
