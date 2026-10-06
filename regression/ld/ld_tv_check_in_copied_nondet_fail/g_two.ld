<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0200">
  <fileHeader companyName="ESBMC" productVersion="1.0" creationDateTime="2026-10-06"/>
  <contentHeader name="TwoIn"/>
  <types/><instances/>
  <pous>
    <pou name="TwoIn" pouType="program">
      <interface>
        <inputVars><variable name="In1"><type><BOOL/></type></variable><variable name="In2"><type><BOOL/></type></variable></inputVars>
        <localVars><variable name="Out1"><type><BOOL/></type></variable></localVars>
      </interface>
      <body>
        <LD>
          <rung localId="1"><contact variable="In1"/><contact variable="In2" negated="negated"/><coil variable="Out1"/></rung>
        </LD>
      </body>
    </pou>
  </pous>
</project>
