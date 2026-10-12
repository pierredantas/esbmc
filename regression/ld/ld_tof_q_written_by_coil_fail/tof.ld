<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- A reset coil on the TOF's Q would end the converter's countdown, while
           MATIEC's instance keeps Q and copies it back on the next scan. -->
      <interface>
        <inputVars>
          <variable name="x"><type><BOOL/></type></variable>
          <variable name="r"><type><BOOL/></type></variable>
        </inputVars>
        <localVars>
          <variable name="q"><type><BOOL/></type></variable>
          <variable name="et"><type><INT/></type></variable>
          <variable name="p"><type><INT/></type><initialValue><simpleValue value="3"/></initialValue></variable>
        </localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><block typeName="TOF" instanceName="t1"><variable formalParameter="IN">x</variable><variable formalParameter="PT">p</variable><variable formalParameter="Q">q</variable><variable formalParameter="ET">et</variable></block></rung>
          <rung localId="2"><contact variable="r"/><coil kind="reset" variable="q"/></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
