varying vec4 vPosition;

uniform float time;
uniform float value;
uniform int isHover;

float map(float vin, float vinMin, float vinMax, float voutMin, float voutMax) {
  return ((vin-vinMin)/(vinMax-vinMin)) * (voutMax-voutMin) + voutMin;
}
void main()
{
    float resX = 288.0;
    vec2 pos;
    pos.x = map(abs(gl_FragCoord.x-resX), 0.0, resX-20.0, 0.0, 1.0);
    pos.y = vPosition.y * 48.0/288.0;

    float dist = smoothstep(0.15, 0.05, distance(pos, vec2(value, 0.0)));
    float dist2 = smoothstep(0.15, 0.05, distance(-pos, vec2(value, 0.0)));

    dist = step(0.95 + sin(time*2.0)*0.05, dist + dist2);
    float w = dist;

    // line
    float l = smoothstep(0.01, 0.0, abs(pos.y));
    l *= smoothstep(value, value-0.01, abs(pos.x));

    float hover = float(isHover)>0.5?0.95:1.0;
    gl_FragColor = vec4(vec3(hover), (w + l)*hover);
}