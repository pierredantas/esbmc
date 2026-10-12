<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- MATIEC's TOF starts with Q false; with q declared TRUE the converter's
           hold arm would count down from scan 0 although IN never rises. -->
      <interface>
        <inputVars><variable name="a"><type><BOOL/></type></variable></inputVars>
        <localVars>
          <variable name="off"><type><BOOL/></type></variable>
          <variable name="q"><type><BOOL/></type><initialValue><simpleValue value="TRUE"/></initialValue></variable>
          <variable name="et"><type><INT/></type></variable>
          <variable name="p"><type><INT/></type><initialValue><simpleValue value="3"/></initialValue></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><contact variable="a"/><contact variable="a" negated="true"/><coil variable="off"/></rung>
          <rung localId="2"><block typeName="TOF" instanceName="t1"><variable formalParameter="IN">off</variable><variable formalParameter="PT">p</variable><variable formalParameter="Q">q</variable><variable formalParameter="ET">et</variable></block></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
