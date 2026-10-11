<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0200">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-11"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <interface>
        <inputVars><variable name="IN1"><type><BOOL/></type></variable></inputVars>
        <localVars>
          <variable name="Never"><type><BOOL/></type></variable>
          <variable name="Q1"><type><BOOL/></type></variable>
          <variable name="ET1"><type><INT/></type></variable>
          <variable name="PT1"><type><INT/></type><initialValue><simpleValue value="3"/></initialValue></variable>
          <variable name="D"><type><INT/></type><initialValue><simpleValue value="-1"/></initialValue></variable>
          <variable name="CV2"><type><INT/></type><initialValue><simpleValue value="0"/></initialValue></variable>
          <variable name="Ok"><type><BOOL/></type><initialValue><simpleValue value="TRUE"/></initialValue></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><block typeName="TOF" instanceName="t1"><variable formalParameter="IN">IN1</variable><variable formalParameter="PT">PT1</variable><variable formalParameter="Q">Q1</variable><variable formalParameter="ET">ET1</variable></block></rung>
          <rung localId="2"><block typeName="SUB" instanceName="s1"><variable formalParameter="IN1">ET1</variable><variable formalParameter="IN2">PT1</variable><variable formalParameter="OUT">D</variable></block></rung>
          <rung localId="3"><block typeName="CTU" instanceName="cu2"><variable formalParameter="CU">Never</variable><variable formalParameter="R">Never</variable><variable formalParameter="PV">D</variable><variable formalParameter="Q">Ok</variable><variable formalParameter="CV">CV2</variable></block></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
