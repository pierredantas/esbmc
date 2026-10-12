<?xml version="1.0" encoding="UTF-8"?>
<project xmlns="http://www.plcopen.org/xml/tc6_0201">
  <fileHeader companyName="SAFE-LD" productVersion="1.0" creationDateTime="2026-10-12"/>
  <contentHeader name="P"/>
  <types><dataTypes/><pous>
    <pou name="P" pouType="program">
      <!-- pf2 is not declared: it is the power-flow accumulator the resolver
           generates for contact 2, so the contact would read its own output. -->
      <interface>
        <inputVars><variable name="a"><type><BOOL/></type></variable></inputVars>
        <localVars><variable name="y"><type><BOOL/></type></variable></localVars>
      </interface>
      <body>
        <LD>
          <leftPowerRail localId="1"><position x="0" y="0"/><connectionPointOut/></leftPowerRail>
          <contact localId="2" negated="false" edge="none"><position x="20" y="10"/>
            <connectionPointIn><connection refLocalId="1"/></connectionPointIn><connectionPointOut/><variable>pf2</variable></contact>
          <coil localId="3" negated="false" storage="none"><position x="100" y="10"/>
            <connectionPointIn><connection refLocalId="2"/></connectionPointIn><connectionPointOut/><variable>y</variable></coil>
        </LD>
      </body>
    </pou>
  </pous></types>
  <instances/>
</project>
