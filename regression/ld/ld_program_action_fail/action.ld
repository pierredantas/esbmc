<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types/><instances/>
  <pous>
    <pou name="P" pouType="program">
      <!-- Nothing invokes action Mark, so mark stays false; running the
           action's network every scan would set it. -->
      <interface>
        <inputVars><variable name="go"><type><BOOL/></type></variable></inputVars>
        <localVars>
          <variable name="mark"><type><BOOL/></type></variable>
          <variable name="q"><type><BOOL/></type></variable>
        </localVars>
      </interface>
      <actions>
        <action name="Mark">
          <body><LD><rung localId="9"><coil variable="mark"/></rung></LD></body>
        </action>
      </actions>
      <body>
        <LD>
          <rung localId="1"><contact variable="go"/><coil variable="q"/></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
