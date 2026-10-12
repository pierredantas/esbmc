<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0200">
  <fileHeader companyName="ESBMC" productVersion="1.0" creationDateTime="2026-10-06"/>
  <contentHeader name="Latch"/>
  <types/><instances/>
  <pous>
    <pou name="Latch" pouType="program">
      <interface>
        <inputVars><variable name="In1"><type><BOOL/></type></variable></inputVars>
        <localVars><variable name="Q"><type><BOOL/></type></variable></localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><contact variable="In1"/><coil variable="Q" kind="set"/></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
