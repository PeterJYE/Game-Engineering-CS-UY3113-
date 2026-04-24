#version 330

const float RED_LUM_CONSTANT = 0.2126;
const float GREEN_LUM_CONSTANT = 0.7152;
const float BLUE_LUM_CONSTANT = 0.0722;

uniform sampler2D texture0;
uniform vec2 lightPosition;
uniform float lightRadius;
uniform int useCircleLight;

in vec2 fragTexCoord;
in vec2 fragPosition;

out vec4 finalColor;

const float LINEAR_TERM    = 0.00003; // linear term
const float QUADRATIC_TERM = 0.00003; // quadratic term
const float MIN_BRIGHTNESS = 0.05;    // avoid total darkness

float attenuate(float distance, float linearTerm, float quadraticTerm)
{
    float attenuation = 1.0 / (1.0 + 
                               linearTerm * distance + 
                               quadraticTerm * distance * distance);

    return max(attenuation, MIN_BRIGHTNESS);
}

void main()
{
    float distance = distance(lightPosition, fragPosition);
    vec4 color = texture(texture0, fragTexCoord);

    if (useCircleLight == 1)
    {
        float edge = 32.0;
        float circle = 1.0 - smoothstep(lightRadius - edge, lightRadius, distance);
        finalColor = vec4(color.rgb * circle, color.a);
        return;
    }

    float brightness = attenuate(distance, LINEAR_TERM, QUADRATIC_TERM);
    finalColor = vec4(color.rgb * brightness, color.a);
}
