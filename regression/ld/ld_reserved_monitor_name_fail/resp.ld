<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- A declared __resp_ctr_r1 would become response r1's monitor counter;
           zeroing it every scan would make a failing response verify SAFE. -->
      <interface>
        <inputVars><variable name="t"><type><BOOL/></type></variable></inputVars>
        <localVars>
          <variable name="r"><type><BOOL/></type></variable>
          <variable name="zero"><type><DINT/></type><initialValue><simpleValue value="0"/></initialValue></variable>
          <variable name="__resp_ctr_r1"><type><DINT/></type><initialValue><simpleValue value="0"/></initialValue></variable>
          <variable name="q"><type><BOOL/></type></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><block typeName="MOVE" instanceName="m"><variable formalParameter="IN1">zero</variable><variable formalParameter="OUT">__resp_ctr_r1</variable></block></rung>
          <rung localId="2"><contact variable="t"/><coil variable="q"/></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
